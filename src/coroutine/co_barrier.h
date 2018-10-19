#ifndef CORONET_CO_BARRIER_H
#define CORONET_CO_BARRIER_H

#include <atomic>
#include "co_cond.h"

class CoroutineBarrier {
  CondVar _cv;
  size_t _count;
  std::atomic<size_t> _current;
public:
  CoroutineBarrier(size_t count) : _count(count - 1), _current(0) {}
  void wait();
};

#endif
