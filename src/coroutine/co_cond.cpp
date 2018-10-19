#include "co_cond.h"
#include "scheduler.h"
#include "manager.h"
#include "context.h"

void CoroutineConditionVariable::wait() {
  _mtx.lock();
  Scheduler::get_current_manager()->current()->set_status(Context::Status::IOblocking);
  _wakeup_queue.emplace(std::make_pair(Scheduler::get_current_manager()->current(), Scheduler::get_current_manager()));
  _mtx.unlock();
  Scheduler::get_current_manager()->manager()->resume(Scheduler::get_current_manager()->current());
}
void CoroutineConditionVariable::signal() {
  _mtx.lock();
  if (!_wakeup_queue.empty()) {
    _wakeup_queue.front().first->set_status(Context::Status::ready);
    _wakeup_queue.front().second->wake_up();
    _wakeup_queue.pop();
  }
  _mtx.unlock();
}
void CoroutineConditionVariable::broadcast() {
  _mtx.lock();
  while (!_wakeup_queue.empty()) {
    _wakeup_queue.front().first->set_status(Context::Status::ready);
    _wakeup_queue.front().second->wake_up();
    _wakeup_queue.pop();
  }
  _mtx.unlock();
}