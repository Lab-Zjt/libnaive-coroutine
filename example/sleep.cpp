#include "coroutine/go.h"

Coro_Main(argc, argv) {
  for (int i = 0; i < 10; ++i) {
    go([i]() {
      for (int j = 0; j < 100; ++j) {
        sleep(i);
        printf("Sleep %ds.\n", i);
      }
    });
  }
  go([]() {
    for (int i = 0; i < 1000; ++i) {
      printf("Current : %ds.\n", i);
      sleep(1);
    }
  });
  for (int i = 0; i < 100; ++i) {
    sleep(2);
    printf("Sleep 2s.\n");
  }
}