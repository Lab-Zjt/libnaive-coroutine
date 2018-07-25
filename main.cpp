#include "src/go.h"
#include <cassert>
#include <zconf.h>
#include <thread>
#include <iostream>
#include <syscall.h>

void func1(std::uint64_t max) {
  std::uint64_t sum = 0;
  for (int i = 0; i < max; ++i) {
    sum += i;
    if (i % 1000000 == 0) {
      printf("func1: %lu\n", sum);
    }
  }
}
void func2(std::uint64_t max) {
  std::uint64_t sum = 0;
  for (int i = 0; i < max; ++i) {
    sum += i;
    if (i % 1000000 == 0) {
      printf("func2: %lu\n", sum);
    }
  }
}
void func4(std::uint64_t max) {
  std::uint64_t sum = 0;
  for (int i = 0; i < max; ++i) {
    sum += i;
    if (i % 1000000 == 0) {
      printf("func4: %lu\n", sum);
    }
  }
  go(func1,1000000000);
}
Coro_Main() {
  go(func1, 1000000000);
  go(func2, 1500000000);
  go([](std::uint64_t max) {
    std::uint64_t sum = 0;
    for (int i = 0; i < max; ++i) {
      sum += i;
      if (i % 1000000 == 0) {
        printf("func3: %lu\n", sum);
      }
    }
  }, 1000000000);
  go([]() {
    func4(1000000000);
  });
}