#include <coroutine/co_cond.h>
#include <utility/io.h>
#include "coroutine/go.h"

using namespace std;
using namespace srlib;

CondVar cv;
CoroutineMutex mut;
int counter = 0;
Coro_Main(argc, argv) {
  go([]() {
    while (true) {
      sleep(1);
      cv.broadcast();
      println("Broadcast.");
      sleep(1);
      cv.signal();
      println("Signal");
      mut.lock();
      counter++;
      if (counter >= 20) {
        sleep(1);
        cv.broadcast();
        mut.unlock();
        break;
      }
      mut.unlock();
    }
    println("Signal End.");
  });
  for (int i = 0; i < 10; ++i) {
    usleep(100);
    go([i]() {
      int times = 0;
      while (true) {
        println(i, "Wait.");
        cv.wait();
        times++;
        println(i, "Wake Up.");
        mut.lock();
        if (counter >= 20) {
          mut.unlock();
          break;
        }
        mut.unlock();
      }
      println(i, "Wake Up", times, "Times.");
    });
  }
}