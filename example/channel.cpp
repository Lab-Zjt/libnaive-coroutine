#include <coroutine/go.h>
#include <coroutine/channel.h>

Coro_Main(argc, argv) {
  Channel<int> chan(1);
  go([&chan]() {
    for (int i = 0; i < 1000; ++i) {
      chan << i;
    }
  });
  go([&chan]() {
    for (int i = 10000; i < 11000; ++i) {
      chan << i;
    }
  });
  for (int i = 0; i < 2000; ++i) {
    int j;
    chan >> j;
    printf("Receive %d From Channel.\n", j);
  }
}