#include "coroutine/go.h"

Coro_Main(argc, argv) {
  for (int i = 1; i < 10; ++i) {
    go([i]() {
      for (int j = 0; j < 100; ++j) {
        sleep(i);
      }
    });
  }
  for (int i = 0; i < 100; ++i) {
    sleep(2);
  }
}