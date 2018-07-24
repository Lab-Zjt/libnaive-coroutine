#include "go.h"

void func1(std::uint64_t max) {
  std::uint64_t sum = 0;
  for (int i = 0; i < max; ++i) {
    sum += i;
    if (i % 100000 == 0) {
      printf("func1: %lu\n", sum);
    }
  }
}
void func2(std::uint64_t max) {
  std::uint64_t sum = 0;
  for (int i = 0; i < max; ++i) {
    sum += i;
    if (i % 100000 == 0) {
      printf("func2: %lu\n", sum);
    }
  }
}
Coro_Main() {
  go(func1, 10000000);
  go(func2, 15000000);
}