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
  tv = new itimerspec;
  tv->it_value.tv_sec = s;
  tv->it_value.tv_nsec = ns;
  tv->it_interval.tv_sec = s;
  tv->it_interval.tv_nsec = ns;
  old = new itimerspec;
  zero = new itimerspec;
  memset(zero, 0, sizeof(itimerspec));
  sig = new sigevent;
  memset(sig, 0, sizeof(sigevent));
  sig->sigev_notify = SIGEV_SIGNAL;
  sig->sigev_signo = SIGVTALRM;
  fprintf(stderr, "thread %lu is %d\n", pthread_self(), signo - 40);
  sig->sigev_value.sival_ptr = mng;
  if (-1 == timer_create(CLOCK_THREAD_CPUTIME_ID, sig, &timerid)) {
    perror("timer_create");
  }
}
void Timer::start() {
  timer_settime(timerid, 0, tv, nullptr);
}
void Timer::stop() {
  timer_settime(timerid, 0, zero, old);
}
void Timer::restart() {
  timer_settime(timerid, 0, old, nullptr);
}
Timer::~Timer() {
  delete tv;
  delete sig;
  delete old;
  delete zero;
}