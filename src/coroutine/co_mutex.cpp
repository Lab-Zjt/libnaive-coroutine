#include "co_mutex.h"
#include "scheduler.h"
#include "context.h"

void CoroutineMutex::lock() {
  auto mng = Scheduler::get_current_manager();
  auto cur = mng->current();
  cur->set_status(Context::Status::ready);
  while (!_mtx.try_lock()) {
    mng->manager()->resume(cur);
  }
  cur->set_status(Context::Status::running);
}
void CoroutineMutex::unlock() {
  _mtx.unlock();
}