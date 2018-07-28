#ifndef NAIVECORO_TIMER_H
#define NAIVECORO_TIMER_H

#define SIG_TM_0 50
#define SIG_TM_1 51
#define SIG_TM_2 52
#define SIG_TM_3 53
#define SIG_TM_4 54
#define SIG_TM_5 55
#define SIG_TM_6 56
#define SIG_TM_7 57
#define SIG_TM_8 58
#define SIG_TM_9 59


struct itimerspec;
struct sigevent;
struct itimerval;
typedef void *timer_t;

class ContextManager;

class Timer {
private:
  itimerspec *tv;
  sigevent *sig;
  timer_t timerid;
  itimerspec *old;
  itimerspec *zero;
public:
  ~Timer();
  Timer(int s, int ns,int tid,ContextManager* mng);
  void start();
  void stop();
  void restart();
};

#endif
