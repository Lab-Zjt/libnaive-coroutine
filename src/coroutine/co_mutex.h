#ifndef CORONET_CO_MUTEX_H
#define CORONET_CO_MUTEX_H

#include <mutex>

class CoroutineMutex {
private:
  std::mutex _mtx;
public:
  CoroutineMutex() = default;
  void lock();
  void unlock();
};

#endif
