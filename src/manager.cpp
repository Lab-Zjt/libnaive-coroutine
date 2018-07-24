#include "manager.h"
#include "timer.h"
#include "context.h"
#include <csignal>

ContextManager *mng = nullptr;
ContextManager::ContextManager() {
  signal(SIGVTALRM, ContextManager::alarm);
  manager = new Context(ContextManager::manage);
  timer = new Timer(0, 1);
}
void ContextManager::alarm(int signo) {
  switch (signo) {
    case SIGVTALRM : {
      mng->manager->resume(mng->cur);
      break;
    }
    default: {
    }
  }
}
void ContextManager::start() {
  timer->start();
  manager->resume(nullptr);
}
void ContextManager::erase(Context *ctx) {
  waiting_for_erase.push_back(ctx);
}
void ContextManager::manage() {
  while (true) {
    if (mng->CtxQueue.empty()) {
      exit(0);
    } else {
      auto end = mng->CtxQueue.end();
      for (auto it = mng->CtxQueue.begin(); it != end; ++it) {
        mng->cur = *it;
        mng->cur->resume(mng->manager);
      }
      int i = 0;
      for (auto it = mng->CtxQueue.begin(); it != mng->CtxQueue.end() && i < mng->waiting_for_erase.size();) {
        if (*it == mng->waiting_for_erase[i]) {
          delete *it;
          ++i;
          it = mng->CtxQueue.erase(it);
          continue;
        } else {
          ++it;
        }
      }
      mng->waiting_for_erase.clear();
    }
  }
}