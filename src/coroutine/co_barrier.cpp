#include <utility/io.h>
#include "co_barrier.h"

void CoroutineBarrier::wait() {
  if (_count == _current) {
    _current = 0;
    _cv.broadcast();
  } else {
    _current++;
    srlib::println("current:", _current);
    _cv.wait();
  }
}