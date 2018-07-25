#include "manager.h"
#include "timer.h"
#include "context.h"
#include "scheduler.h"
#include <csignal>
#include <zconf.h>
#include <cstring>

ContextManager::ContextManager(int index) : empty(true), running(false), signo(40 + index) {
  perror("signal");
  fprintf(stderr, "%d:sig%d\n", index, signo);
  manager = new Context(&ContextManager::manage, this);
}
void ContextManager::setSignal() {
  signal(signo, ContextManager::alarm);
}
void ContextManager::setmask() {
  sigset_t ss;
  sigfillset(&ss);
  sigdelset(&ss, signo);
  pthread_sigmask(SIG_BLOCK, &ss, nullptr);
}
void ContextManager::alarm(int signo) {
  auto mng = Scheduler::getCurrentManager();
  if (signo == mng->signo) {
    mng->manager->resume(mng->cur);
  }
}
void ContextManager::start() {
  timer = new Timer(0, 5000, signo);
  manager->resume(nullptr);
}
void ContextManager::erase(Context *ctx) {
  waiting_for_erase.push_back(ctx);
}
void ContextManager::queue_to_ctx() {
  std::lock_guard<std::mutex> lock(mtx);
  for (auto &it:queue) {
    CtxQueue.emplace_back(new Context(it));
  }
  queue.clear();
}
void ContextManager::manage() {
  while (true) {
    queue_to_ctx();
    if (CtxQueue.empty()) {
      empty = true;
      usleep(10);
    } else {
      empty = false;
      fprintf(stderr, "%lu task in %d\n", CtxQueue.size(), signo - 40);
      timer->start();
      for (auto it = CtxQueue.begin(); it != CtxQueue.end(); ++it) {
        cur = *it;
        cur->resume(manager);
      }
      int i = 0;
      timer->stop();
      for (auto it = CtxQueue.begin(); it != CtxQueue.end() && i < waiting_for_erase.size();) {
        if (*it == waiting_for_erase[i]) {
          delete *it;
          ++i;
          it = CtxQueue.erase(it);
          continue;
        } else {
          ++it;
        }
      }
      waiting_for_erase.clear();
    }
  }
  printf("I break...\n");
}