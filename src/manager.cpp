#include "manager.h"
#include "timer.h"
#include "context.h"
#include "scheduler.h"
#include <csignal>
#include <zconf.h>
#include <cstring>

ContextManager::ContextManager(int index) : empty(true), running(false), signo(40 + index) {
  manager = new Context(&ContextManager::manage, this);
}
void ContextManager::setSignal() {
  signal(signo, ContextManager::alarm);
}
void ContextManager::alarm(int signo) {
  auto mng = Scheduler::getCurrentManager();
  if (signo == mng->signo) {
    if (mng->cur->status == Context::Status::syscalling) {
      return;
    }
    mng->cur->status = Context::Status::ready;
    mng->manager->resume(mng->cur);
  }
}
void ContextManager::start() {
  timer = new Timer(0, 5000000, signo, this);
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
      printf("%d switch to ready.\n", signo - 40);
      status = Status::ready;
      usleep(100);
    } else {
      empty = false;
      printf("%lu task in %d\n", CtxQueue.size(), signo - 40);
      int i = 0;
      for (auto it = CtxQueue.begin(); it != CtxQueue.end();) {
        auto ctx = *it;
        if (ctx->status == Context::Status::ready) {
          ctx->status = Context::Status::running;
          printf("%d switch to running.\n", signo - 40);
          this->status = Status::running;
          this->cur = ctx;
          timer->start();
          ctx->resume(manager);
          timer->stop();
          this->cur = nullptr;
          ++it;
        } else if (ctx->status == Context::Status::IOblocking) {
          ++it;
          continue;
        } else if (ctx->status == Context::Status::finished) {
          it = CtxQueue.erase(it);
          continue;
        }
      }
    }
  }
  printf("I break...\n");
}