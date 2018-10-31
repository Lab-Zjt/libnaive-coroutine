#ifndef CORONET_CHANNEL_H
#define CORONET_CHANNEL_H

#include <queue>
#include "co_cond.h"

template<typename T>
class Channel {
private:
  std::queue<T> _queue;
  CondVar _cv;
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
        _mtx.unlock();
        _cv.wait();
      } else {
        break;
      }
    }
    if (_queue.size() == 0) {
      _cv.signal();
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
        _mtx.unlock();
        _cv.wait();
      } else break;
    }
    T tmp = std::move(_queue.front());
    if (_queue.size() == _capacity) {
      _cv.signal();
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
        _mtx.unlock();
        _cv.wait();
      } else {
        break;
      }
    }
    if (_queue.size() == 0) {
      _cv.signal();
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
        rhs._mtx.unlock();
        rhs._cv.wait();
      } else break;
    }
    lhs = std::move(rhs._queue.front());
    if (rhs._queue.size() == rhs._capacity) {
      rhs._cv.signal();
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
        lhs._mtx.unlock();
        lhs._cv.wait();
      } else break;
    }
    rhs = std::move(lhs._queue.front());
    if (lhs._queue.size() == lhs._capacity) {
      lhs._cv.signal();
    }
    lhs._queue.pop();
    lhs._mtx.unlock();
  }
};

#endif
