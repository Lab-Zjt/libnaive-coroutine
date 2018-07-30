#include <coroutine/go.h>

void func(int index) {
  //set timer to 1ns to check override operator new and delete work or not.
  for (int i = 0; i < 10000; ++i) {
    auto ptr = new char[64 * 1024 * 1024];
    printf("func %d new %d bytes.\n", index, 64 * 1024 * 1024);
    delete[]ptr;
    printf("func %d delete %d bytes.\n", index, 64 * 1024 * 1024);
  }
  //set timer to 1ns to check hook malloc and free work or not.
  for (int i = 0; i < 10000; ++i) {
    auto ptr = malloc(64 * 1024 * 1024);
    printf("func %d malloc %d bytes.\n", index, 64 * 1024 * 1024);
    free(ptr);
    printf("func %d free %d bytes.\n", index, 64 * 1024 * 1024);
  }
}
Coro_Main(argc, argv) {
  go(func, 0);
  go(func, 1);
  go(func, 2);
  go(func, 3);
  go(func, 4);
  go(func, 5);
  go(func, 6);
  go(func, 7);
  go(func, 8);
  go(func, 9);
}