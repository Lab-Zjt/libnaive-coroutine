#include "co_mutex.h"
#include "scheduler.h"
#include "context.h"
#include "co_yield.h"

void CoroutineMutex::lock()  {
  while (!_mtx.try_lock()) {
    co_yield;
  }
}
void CoroutineMutex::unlock() {
  _mtx.unlock();
}