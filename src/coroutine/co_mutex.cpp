#include "co_mutex.h"
#include "scheduler.h"
#include "context.h"

void CoroutineMutex::lock() {
  auto mng = Scheduler::get_current_manager();
  auto cur = mng->current();
  auto save = cur->status();
  cur->set_status(Context::Status::syscalling);
  while (!_mtx.try_lock()) {
    mng->manager()->resume(cur);
  }
  cur->set_status(save);
}
void CoroutineMutex::unlock() {
  auto mng = Scheduler::get_current_manager();
  auto cur = mng->current();
  auto save = cur->status();
  cur->set_status(Context::Status::syscalling);
  _mtx.unlock();
  cur->set_status(save);
}