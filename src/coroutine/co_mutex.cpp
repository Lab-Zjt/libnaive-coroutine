#include "co_mutex.h"
#include "scheduler.h"
#include "context.h"
#include "go.h"

void CoroutineMutex::lock()  {
  while (!_mtx.try_lock()) {
    co_yiled;
  }
}
void CoroutineMutex::unlock() {
  _mtx.unlock();
}