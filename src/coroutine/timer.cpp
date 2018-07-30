#include "timer.h"
#include "manager.h"
#include <ctime>
#include <cstring>
#include <csignal>
#include <cstdio>
#include <sys/time.h>
#include <bits/types/sigevent_t.h>
#include <bits/types/siginfo_t.h>
#include <pthread.h>

Timer::Timer(int s, int ns, int signo,ContextManager* mng) {
  _tv = new itimerspec;
  _tv->it_value.tv_sec = s;
  _tv->it_value.tv_nsec = ns;
  _tv->it_interval.tv_sec = s;
  _tv->it_interval.tv_nsec = ns;
  _old = new itimerspec;
  _zero = new itimerspec;
  memset(_zero, 0, sizeof(itimerspec));
  _sig = new sigevent;
  memset(_sig, 0, sizeof(sigevent));
  _sig->sigev_notify = SIGEV_SIGNAL;
  _sig->sigev_signo = SIGVTALRM;
  _sig->sigev_value.sival_ptr = mng;
  if (-1 == timer_create(CLOCK_THREAD_CPUTIME_ID, _sig, &_timerid)) {
    perror("timer_create");
  }
}
void Timer::start() {
  timer_settime(_timerid, 0, _tv, nullptr);
}
void Timer::stop() {
  timer_settime(_timerid, 0, _zero, _old);
}
void Timer::restart() {
  timer_settime(_timerid, 0, _old, nullptr);
}
Timer::~Timer() {
  delete _tv;
  delete _sig;
  delete _old;
  delete _zero;
}