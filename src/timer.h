#ifndef NAIVECORO_TIMER_H
#define NAIVECORO_TIMER_H

struct itimerval;

class Timer {
private:
  itimerval *tv;
public:
  ~Timer();
  Timer(int s, int us);
  void start();
};

#endif
