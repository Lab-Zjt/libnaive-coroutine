#include "context.h"
#include "scheduler.h"
#include <pthread.h>
#include <csignal>

int coro_main(int argc, char *argv[]);

#define Main() \
int main(int argc,char* argv[]){\
  sigset_t ss;\
  sigemptyset(&ss);\
  sigaddset(&ss,SIGVTALRM);\
  pthread_sigmask(SIG_BLOCK, &ss, nullptr);\
  _scheduler = new Scheduler;\
  _scheduler->start(coro_main,argc,argv);\
  return 0;\
}

#define Coro_Main() Main();\
int coro_main(int argc, char *argv[])
