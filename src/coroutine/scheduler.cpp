#include "scheduler.h"
#include "manager.h"
#include "context.h"
#include <unistd.h>
#include <csignal>

Scheduler *_scheduler = nullptr;
Scheduler::Scheduler() : _index(0), _initializing(true) {}
void Scheduler::init() {
  //create context manager.
  for (int i = 0; i < max_thread; ++i) {
    _manager[i] = new ContextManager(i);
  }
  _initializing = false;
}
void Scheduler::push_func(std::function<void()> &&func, size_t stack_size) {
  //Remove mutex, hope it would not trigger bugs.
  //mtx.lock();
  //select a manager, and it is stupid because sometimes all heavy task would push into a queue.
  auto mng = _manager[_index++];
  if (_index >= max_thread) {
    _index = 0;
  }
  if (mng->status() == ContextManager::Status::creating) {
    mng->set_status(ContextManager::Status::running);
    //if func pass by reference may cause some problem, although I'm not sure the problem is caused by pass by reference.
    std::thread([mng, stack_size](std::function<void()> &&fn) {
      //mtx.unlock();
      mng->push_to_queue(std::move(fn), stack_size);
      mng->set_tid(pthread_self());
      mng->start();
    }, std::move(func)).detach();
  } else {
    //mtx.unlock();
    mng->push_to_queue(std::move(func), stack_size);
  }
}
ContextManager *Scheduler::get_current_manager() {
  if (_scheduler == nullptr) {
    return nullptr;
  }
  if (_scheduler->_initializing) {
    return nullptr;
  }
  auto tid = pthread_self();
  //find manager by tid
  for (int i = 0; i < max_thread; ++i) {
    if (tid == _scheduler->_manager[i]->tid()) {
      return _scheduler->_manager[i];
    }
  }
  return nullptr;
}
void Scheduler::start(main_t cmain, int argc, char *argv[]) {
  init();
  //push main function to queue.
  push_func(std::bind(cmain, argc, argv), 1024 * 1024);
  int sig;
  //TODO : I didn't find a appropriate time slice length, so I hard code it now.
  // this thread will check each manager per 50ms, if all manager is empty, the process will exit.
  timespec tv{};
  tv.tv_sec = 50000;
  tv.tv_nsec = 50000;
  siginfo_t st;
  sigset_t ss;
  sigaddset(&ss, SIGVTALRM);
  while (true) {
    //wait signal.
    if ((sig = sigtimedwait(&ss, &st, &tv)) < 0) {
      //if no timer is running, check if all queue is empty, if empty, then exit.
      if (errno == EAGAIN) {
        //if not capture any signal, check queue is empty or not.
        //if empty, it means all task is finished, exit.
        if (empty()) {
          exit(0);
        }
      }
    }
  }
}
bool Scheduler::empty() {
  //check queue is empty or not.
  for (int i = 0; i < max_thread; ++i) {
    if (_manager[i]->status() == ContextManager::Status::running) {
      return false;
    }
  }
  return true;
}