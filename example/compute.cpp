#include <coroutine/go.h>
#include <cassert>
#include <zconf.h>
#include <thread>
#include <omp.h>
#include <iostream>
#include <syscall.h>

void func1(std::uint64_t max) {
  std::uint64_t sum = 0;
  for (int i = 0; i < max; ++i) {
    sum += i;
    if (i % 1000000 == 0) {
      printf("func1: %lu\r", sum);
    }
    if (i % 500000 == 0){
      co_yiled;
    }
  }
}
void func2(std::uint64_t max) {
  std::uint64_t sum = 0;
  for (int i = 0; i < max; ++i) {
    sum += i;
    if (i % 1000000 == 0) {
      printf("func2: %lu\r", sum);
    }
  }
}
void func4(std::uint64_t max) {
  std::uint64_t sum = 0;
  for (int i = 0; i < max; ++i) {
    sum += i;
    if (i % 1000000 == 0) {
      printf("func4: %lu\r", sum);
    }
  }
  go(func1, 100000000);
}
Coro_Main(argc, argv) {
  std::cout << "hello,world" << std::endl;
  go(func1, 100000000);
  go(func2, 150000000);
  go([](std::uint64_t max) {
    std::uint64_t sum = 0;
    for (int i = 0; i < max; ++i) {
      sum += i;
      if (i % 1000000 == 0) {
        printf("func3: %lu\r", sum);
      }
    }
  }, 100000000);
  go([]() {
    func4(100000000);
  });
  go(func2, 100000000);
  go(func1, 100000000);
  go(func2, 100000000);
  go(func1, 100000000);
}