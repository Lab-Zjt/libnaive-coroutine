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
  inline void operator<<(const T &data);
  // operator() fetch a element from the channel.
  // Usage: auto data = channel()
  inline T operator()();
  // operator<<(element) send a element to the channel.
  // Usage: Channel << element.
  inline void operator<<(T &&data);
  // operator<<(channel,data) fetch a element from channel and set data's value as element.
  // Usage: channel >> data
  inline void operator>>(T &rhs);
};

#include "channel.inl"
#endif
