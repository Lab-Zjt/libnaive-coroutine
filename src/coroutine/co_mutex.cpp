#include "co_mutex.h"
#include "scheduler.h"
#include "context.h"
#include "co_yield.h"

static uint64_t wd_buf = 1;
static uint64_t rd_buf = 0;

void CoroutineMutex::lock() {
  while (!_mtx.try_lock()) {
    co_yield;
  }
}
void CoroutineMutex::unlock() {
  _mtx.unlock();
}