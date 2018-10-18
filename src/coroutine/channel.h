#ifndef CORONET_CHANNEL_H
#define CORONET_CHANNEL_H

#include <queue>
#include <sys/epoll.h>
#include <utility/io.h>
#include "go.h"
#include "co_mutex.h"
#include "co_yield.h"

const static uint64_t wr_buf = 1;
static uint64_t rd_buf = 0;

template<typename T>
class Channel {
private:
  std::queue<T> _queue;
  std::queue<int> _wakeup_queue;
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
    int evfd = eventfd(0, 0);
    while (true) {
      _mtx.lock();
      if (_queue.size() >= _capacity) {
        epoll_event ev{};
        ev.events = EPOLLIN;
        ev.data.ptr = Scheduler::get_current_manager()->current();
        epoll_ctl(Scheduler::get_current_manager()->epfd(), EPOLL_CTL_ADD, evfd, &ev);
        Scheduler::get_current_manager()->current()->set_status(Context::Status::IOblocking);
        _wakeup_queue.push(evfd);
        _mtx.unlock();
        Scheduler::get_current_manager()->manager()->resume(Scheduler::get_current_manager()->current());
        Scheduler::get_current_manager()->current()->set_status(Context::Status::running);
        read(evfd, &rd_buf, sizeof(rd_buf));
      } else {
        break;
      }
    }
    close(evfd);
    if (_queue.size() == 0 && _wakeup_queue.size() != 0) {
      write(_wakeup_queue.front(), &wr_buf, sizeof(wr_buf));
      _wakeup_queue.pop();
    }
    _queue.push(data);
    _mtx.unlock();
  }
  // operator() fetch a element from the channel.
  // Usage: auto data = channel()
  T operator()() {
    int evfd = eventfd(0, 0);
    while (true) {
      _mtx.lock();
      if (_queue.size() == 0) {
        epoll_event ev{};
        ev.events = EPOLLIN;
        ev.data.ptr = Scheduler::get_current_manager()->current();
        epoll_ctl(Scheduler::get_current_manager()->epfd(), EPOLL_CTL_ADD, evfd, &ev);
        Scheduler::get_current_manager()->current()->set_status(Context::Status::IOblocking);
        _wakeup_queue.push(evfd);
        _mtx.unlock();
        Scheduler::get_current_manager()->manager()->resume(Scheduler::get_current_manager()->current());
        Scheduler::get_current_manager()->current()->set_status(Context::Status::running);
        read(evfd, &rd_buf, sizeof(rd_buf));
      } else break;
    }
    close(evfd);
    T tmp = std::move(_queue.front());
    if (_queue.size() == _capacity && _wakeup_queue.size() != 0) {
      write(_wakeup_queue.front(), &wr_buf, sizeof(wr_buf));
      _wakeup_queue.pop();
    }
    _queue.pop();
    _mtx.unlock();
    return tmp;
  }
  // operator<<(element) send a element to the channel.
  // Usage: Channel << element.
  void operator<<(T &&data) {
    int evfd = eventfd(0, 0);
    while (true) {
      _mtx.lock();
      if (_queue.size() >= _capacity) {
        epoll_event ev{};
        ev.events = EPOLLIN;
        ev.data.ptr = Scheduler::get_current_manager()->current();
        epoll_ctl(Scheduler::get_current_manager()->epfd(), EPOLL_CTL_ADD, evfd, &ev);
        Scheduler::get_current_manager()->current()->set_status(Context::Status::IOblocking);
        _wakeup_queue.push(evfd);
        _mtx.unlock();
        Scheduler::get_current_manager()->manager()->resume(Scheduler::get_current_manager()->current());
        Scheduler::get_current_manager()->current()->set_status(Context::Status::running);
        read(evfd, &rd_buf, sizeof(rd_buf));
      } else {
        break;
      }
    }
    close(evfd);
    if (_queue.size() == 0 && _wakeup_queue.size() != 0) {
      write(_wakeup_queue.front(), &wr_buf, sizeof(wr_buf));
      _wakeup_queue.pop();
    }
    _queue.emplace(data);
    _mtx.unlock();
  }
  // operator<<(data,channel) fetch a element from channel and set data's value as element.
  // Usage: data << channel
  friend void operator<<(T &lhs, Channel<T> &rhs) {
    int evfd = eventfd(0, 0);
    while (true) {
      rhs._mtx.lock();
      if (rhs._queue.size() == 0) {
        epoll_event ev{};
        ev.events = EPOLLIN;
        ev.data.ptr = Scheduler::get_current_manager()->current();
        epoll_ctl(Scheduler::get_current_manager()->epfd(), EPOLL_CTL_ADD, evfd, &ev);
        Scheduler::get_current_manager()->current()->set_status(Context::Status::IOblocking);
        rhs._wakeup_queue.push(evfd);
        rhs._mtx.unlock();
        Scheduler::get_current_manager()->manager()->resume(Scheduler::get_current_manager()->current());
        Scheduler::get_current_manager()->current()->set_status(Context::Status::running);
        read(evfd, &rd_buf, sizeof(rd_buf));
      } else break;
    }
    close(evfd);
    lhs = std::move(rhs._queue.front());
    if (rhs._queue.size() == rhs._capacity && rhs._wakeup_queue.size() != 0) {
      write(rhs._wakeup_queue.front(), &wr_buf, sizeof(wr_buf));
      rhs._wakeup_queue.pop();
    }
    rhs._queue.pop();
    rhs._mtx.unlock();
  }
  // operator<<(channel,data) fetch a element from channel and set data's value as element.
  // Usage: channel >> data
  friend void operator>>(Channel<T> &lhs, T &rhs) {
    int evfd = eventfd(0, 0);
    while (true) {
      lhs._mtx.lock();
      if (lhs._queue.size() == 0) {
        epoll_event ev{};
        ev.events = EPOLLIN;
        ev.data.ptr = Scheduler::get_current_manager()->current();
        epoll_ctl(Scheduler::get_current_manager()->epfd(), EPOLL_CTL_ADD, evfd, &ev);
        Scheduler::get_current_manager()->current()->set_status(Context::Status::IOblocking);
        lhs._wakeup_queue.push(evfd);
        lhs._mtx.unlock();
        Scheduler::get_current_manager()->manager()->resume(Scheduler::get_current_manager()->current());
        Scheduler::get_current_manager()->current()->set_status(Context::Status::running);
        read(evfd, &rd_buf, sizeof(rd_buf));
      } else break;
    }
    close(evfd);
    rhs = std::move(lhs._queue.front());
    if (lhs._queue.size() == lhs._capacity && lhs._wakeup_queue.size() != 0) {
      write(lhs._wakeup_queue.front(), &wr_buf, sizeof(wr_buf));
      lhs._wakeup_queue.pop();
    }
    lhs._queue.pop();
    lhs._mtx.unlock();
  }
};

#endif
