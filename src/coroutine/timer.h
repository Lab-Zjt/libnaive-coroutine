#ifndef NAIVECORO_TIMER_H
#define NAIVECORO_TIMER_H

struct itimerspec;
struct sigevent;
typedef void *timer_t;

class ContextManager;

class Timer {
private:
  itimerspec *_tv;
  sigevent *_sig;
  timer_t _timerid;
  itimerspec *_old;
  itimerspec *_zero;
public:
  ~Timer();
  Timer(int s, int ns, int tid, ContextManager *mng);
  void start();
  void stop();
  void restart();
};

#endif
