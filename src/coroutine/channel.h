#ifndef CORONET_CHANNEL_H
#define CORONET_CHANNEL_H

#include <queue>
#include "go.h"
#include "co_mutex.h"

template<typename T>
class Channel {
private:
  std::queue<T> _queue;
  CoroutineMutex _mtx;
  size_t _capacity = std::numeric_limits<size_t>::max();
public:
  Channel() = default;
  explicit Channel(size_t capacity) : _capacity(capacity) {}
  Channel(const Channel &) = delete;
  Channel &operator=(const Channel &)= delete;
  void operator<<(const T &data) {
    while (true) {
      _mtx.lock();
      if (_queue.size() >= _capacity) {
        _mtx.unlock();
        co_yiled;
      } else {
        break;
      }
    }
    _queue.push(data);
    _mtx.unlock();
  }
  void operator<<(T &&data) {
    while (true) {
      _mtx.lock();
      if (_queue.size() >= _capacity) {
        _mtx.unlock();
        co_yiled;
      } else {
        break;
      }
    }
    _queue.emplace(data);
    _mtx.unlock();
  }
  friend void operator<<(T &lhs, Channel<T> &rhs) {
    while (true) {
      rhs._mtx.lock();
      if (rhs._queue.size() == 0) {
        rhs._mtx.unlock();
        co_yiled;
      } else break;
    }
    lhs = std::move(rhs._queue.front());
    rhs._queue.pop();
    rhs._mtx.unlock();
  }
  friend void operator>>(Channel<T> &lhs, T &rhs) {
    while (true) {
      lhs._mtx.lock();
      if (lhs._queue.size() == 0) {
        lhs._mtx.unlock();
        co_yiled;
      } else break;
    }
    rhs = std::move(lhs._queue.front());
    lhs._queue.pop();
    lhs._mtx.unlock();
  }
};

#endif
