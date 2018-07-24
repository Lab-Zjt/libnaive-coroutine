#include "timer.h"
#include <sys/time.h>

Timer::Timer(int s, int us) {
  tv = new itimerval;
  tv->it_value.tv_sec = s;
  tv->it_value.tv_usec = us;
  tv->it_interval.tv_sec = s;
  tv->it_interval.tv_usec = us;
}
void Timer::start() {setitimer(ITIMER_VIRTUAL, tv, nullptr);}

Timer::~Timer() {delete tv;}