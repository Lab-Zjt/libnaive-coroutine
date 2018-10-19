#ifndef CORONET_CO_COND_H
#define CORONET_CO_COND_H

#include <queue>
#include "co_mutex.h"

class Context;

class ContextManager;

class CoroutineConditionVariable {
  CoroutineMutex _mtx;
  std::queue<std::pair<Context *, ContextManager *>> _wakeup_queue;
public:
  CoroutineConditionVariable() = default;
  void wait();
  void signal();
  void broadcast();
};

typedef CoroutineConditionVariable CondVar;

#endif
