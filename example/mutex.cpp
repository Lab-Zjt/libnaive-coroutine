#include <coroutine/go.h>
#include <coroutine/co_mutex.h>

CoroutineMutex mtx;
std::uint64_t sum = 0;
int count = 0;
Coro_Main(argc, argv) {
  auto f = []() {
    for (int i = 0; i < 1000000; ++i) {
      mtx.lock();
      sum += i;
      mtx.unlock();
    }
    mtx.lock();
    printf("sum = %lu\n", sum);
    count++;
    mtx.unlock();
  };
  go(f);
  go(f);
  go(f);
  go(f);
  for (int i = 0; i < 28; ++i) {
    go(f);
  }
}