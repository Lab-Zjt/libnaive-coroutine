#include "scheduler.h"
#include "manager.h"
#include "context.h"
#include <unistd.h>
#include <csignal>
#include <bits/types/siginfo_t.h>

Scheduler *_scheduler = nullptr;
Scheduler::Scheduler() : index(0), task_count(0) {}
void Scheduler::init() {
  for (int i = 0; i < maxThread; ++i) {
    manager[i] = new ContextManager(i);
  }
  initializing = false;
}
void Scheduler::push_task(Context *ctx) {
  auto mng = manager[index++];
  if (index >= maxThread) {
    index = 0;
  }
  mng->empty = false;
  if (!mng->running) {
    std::thread([=]() {
      mng->running = true;
      mng->setSignal();
      mng->push(ctx);
      mng->tid = pthread_self();
      mng->start();
    }).detach();
  } else {
    mng->push(ctx);
  }
}
void Scheduler::push_main(Scheduler::main_t cmain, int argc, char **argv) {
  auto mng = manager[index++];
  if (index >= maxThread) {
    index = 0;
  }
  mng->empty = false;
  std::thread([=]() {
    mng->running = true;
    mng->push_to_queue(std::bind(cmain, argc, argv));
    mng->setSignal();
    mng->tid = pthread_self();
    mng->start();
  }).detach();
}
void Scheduler::push_func(std::function<void()> &&func) {
  mtx.lock();
  auto mng = manager[index++];
  if (index >= maxThread) {
    index = 0;
  }
  mng->empty = false;
  if (!mng->running) {
    std::thread([mng, this](std::function<void()> &&fn) {
      mtx.unlock();
      mng->running = true;
      mng->push_to_queue(std::move(fn));
      mng->setSignal();
      mng->tid = pthread_self();
      mng->start();
    }, std::move(func)).detach();
  } else {
    mng->push_to_queue(std::move(func));
  }
}
ContextManager *Scheduler::getCurrentManager() {
  if (_scheduler->initializing) {
    return nullptr;
  }
  auto tid = pthread_self();
  for (int i = 0; i < maxThread; ++i) {
    if (tid == _scheduler->manager[i]->tid) {
      return _scheduler->manager[i];
    }
  }
  return nullptr;
}
void Scheduler::start(main_t cmain, int argc, char *argv[]) {
  init();
  //push_main(cmain, argc, argv);
  push_func(std::bind(cmain, argc, argv));
  int sig;
  timespec tv;
  tv.tv_sec = 0;
  tv.tv_nsec = 10000;
  siginfo_t st;
  sigset_t ss;
  sigfillset(&ss);
  while (true) {
    if ((sig = sigtimedwait(&ss, &st, &tv)) < 0) {
      //if no timer is running, check if all queue is empty, if empty, then exit.
      if (errno == EAGAIN) {
        if (empty()) {
          exit(0);
        }
      }
    } else {
      pthread_kill(pthread_t(st._sifields._timer.si_sigval.sival_ptr), sig - 10);
    }
  }
}
bool Scheduler::empty() {
  for (int i = 0; i < maxThread; ++i) {
    if (!manager[i]->running) {
      break;
    }
    if (!manager[i]->empty) {
      return false;
    }
  }
  return true;
}