template<typename T>
void Channel<T>::operator<<(const T &data) {
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
template<typename T>
T Channel<T>::operator()() {
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
template<typename T>
void Channel<T>::operator<<(T &&data) {
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
template<typename T>
void Channel<T>::operator>>(T &rhs) {
  while (true) {
    _mtx.lock();
    if (_queue.size() == 0) {
      _mtx.unlock();
      _cv.wait();
    } else break;
  }
  rhs = std::move(_queue.front());
  if (_queue.size() == _capacity) {
    _cv.signal();
  }
  _queue.pop();
  _mtx.unlock();
}