#include <coroutine/co_barrier.h>
#include <utility/io.h>
#include "coroutine/go.h"

using namespace std;
using namespace srlib;

CoroutineBarrier barrier(16);
Coro_Main(argc, argv) {
  go([]() {
    sleep(20);
    println("Prepare to start...");
    sleep(2);
    barrier.wait();
  });
  for (int i = 0; i < 15; ++i) {
    go([i]() {
      sleep(i);
      println(i, "waiting.");
      barrier.wait();
      println(i, "wakeup!");
    });
  }
}