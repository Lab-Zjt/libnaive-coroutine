#ifndef CORONET_CHANNEL_H
#define CORONET_CHANNEL_H

#include <queue>
#include "co_mutex.h"
#include "scheduler.h"
#include "manager.h"
#include "context.h"

template<typename T>
class Channel {
private:
  std::queue<T> _queue;
  std::queue<std::pair<Context *, ContextManager *>> _wakeup_queue;
  CoroutineMutex _mtx;
  size_t _capacity = std::numeric_limits<size_t>::max();
public:
  Channel() = default;
  explicit Channel(size_t capacity) : _capacity(capacity) {}
  Channel(const Channel &) = delete;
  Channel &operator=(const Channel &)= delete;
  // Get status of the channel.
  bool Empty() {return _queue.empty();}
  // operator<<(data) send a data to the channel.
  // Usage: channel << data.
  void operator<<(const T &data) {
    while (true) {
      _mtx.lock();
      if (_queue.size() >= _capacity) {
        Scheduler::get_current_manager()->current()->set_status(Context::Status::IOblocking);
        _wakeup_queue.emplace(std::make_pair(Scheduler::get_current_manager()->current(),
                                             Scheduler::get_current_manager()));
        _mtx.unlock();
        Scheduler::get_current_manager()->manager()->resume(Scheduler::get_current_manager()->current());
      } else {
        break;
      }
    }
    if (_queue.size() == 0 && _wakeup_queue.size() != 0) {
      _wakeup_queue.front().first->set_status(Context::Status::ready);
      _wakeup_queue.front().second->wake_up();
      _wakeup_queue.pop();
    }
    _queue.push(data);
    _mtx.unlock();
  }
  // operator() fetch a element from the channel.
  // Usage: auto data = channel()
  T operator()() {
    while (true) {
      _mtx.lock();
      if (_queue.size() == 0) {
        Scheduler::get_current_manager()->current()->set_status(Context::Status::IOblocking);
        _wakeup_queue.emplace(std::make_pair(Scheduler::get_current_manager()->current(),
                                             Scheduler::get_current_manager()));
        _mtx.unlock();
        Scheduler::get_current_manager()->manager()->resume(Scheduler::get_current_manager()->current());
      } else break;
    }
    T tmp = std::move(_queue.front());
    if (_queue.size() == _capacity && _wakeup_queue.size() != 0) {
      _wakeup_queue.front().first->set_status(Context::Status::ready);
      _wakeup_queue.front().second->wake_up();
      _wakeup_queue.pop();
    }
    _queue.pop();
    _mtx.unlock();
    return tmp;
  }
  // operator<<(element) send a element to the channel.
  // Usage: Channel << element.
  void operator<<(T &&data) {
    while (true) {
      _mtx.lock();
      if (_queue.size() >= _capacity) {
        Scheduler::get_current_manager()->current()->set_status(Context::Status::IOblocking);
        _wakeup_queue.emplace(std::make_pair(Scheduler::get_current_manager()->current(),
                                             Scheduler::get_current_manager()));
        _mtx.unlock();
        Scheduler::get_current_manager()->manager()->resume(Scheduler::get_current_manager()->current());
      } else {
        break;
      }
    }
    if (_queue.size() == 0 && _wakeup_queue.size() != 0) {
      _wakeup_queue.front().first->set_status(Context::Status::ready);
      _wakeup_queue.front().second->wake_up();
      _wakeup_queue.pop();
    }
    _queue.emplace(data);
    _mtx.unlock();
  }
  // operator<<(data,channel) fetch a element from channel and set data's value as element.
  // Usage: data << channel
  friend void operator<<(T &lhs, Channel<T> &rhs) {
    while (true) {
      rhs._mtx.lock();
      if (rhs._queue.size() == 0) {
        Scheduler::get_current_manager()->current()->set_status(Context::Status::IOblocking);
        rhs._wakeup_queue
           .emplace(std::make_pair(Scheduler::get_current_manager()->current(), Scheduler::get_current_manager()));
        rhs._mtx.unlock();
        Scheduler::get_current_manager()->manager()->resume(Scheduler::get_current_manager()->current());
      } else break;
    }
    lhs = std::move(rhs._queue.front());
    if (rhs._queue.size() == rhs._capacity && rhs._wakeup_queue.size() != 0) {
      rhs._wakeup_queue.front().first->set_status(Context::Status::ready);
      rhs._wakeup_queue.front().second->wake_up();
      rhs._wakeup_queue.pop();
    }
    rhs._queue.pop();
    rhs._mtx.unlock();
  }
  // operator<<(channel,data) fetch a element from channel and set data's value as element.
  // Usage: channel >> data
  friend void operator>>(Channel<T> &lhs, T &rhs) {
    while (true) {
      lhs._mtx.lock();
      if (lhs._queue.size() == 0) {
        Scheduler::get_current_manager()->current()->set_status(Context::Status::IOblocking);
        lhs._wakeup_queue
           .emplace(std::make_pair(Scheduler::get_current_manager()->current(), Scheduler::get_current_manager()));
        lhs._mtx.unlock();
        Scheduler::get_current_manager()->manager()->resume(Scheduler::get_current_manager()->current());
      } else break;
    }
    rhs = std::move(lhs._queue.front());
    if (lhs._queue.size() == lhs._capacity && lhs._wakeup_queue.size() != 0) {
      lhs._wakeup_queue.front().first->set_status(Context::Status::ready);
      lhs._wakeup_queue.front().second->wake_up();
      lhs._wakeup_queue.pop();
    }
    lhs._queue.pop();
    lhs._mtx.unlock();
  }
};

#endif
