#include "context.h"
#include "scheduler.h"
#include "hook/hook.h"
#include <net/tlsconn.h>
#include <pthread.h>
#include <csignal>

int coro_main(int argc, char *argv[]);

#define Main() \
int main(int argc,char* argv[]){\
  hook_all();\
  soranet::SSLinitialize();\
  sigset_t ss;\
  sigemptyset(&ss);\
  sigaddset(&ss,SIGVTALRM);\
  pthread_sigmask(SIG_BLOCK, &ss, nullptr);\
  _scheduler = new Scheduler;\
  _scheduler->start(coro_main,argc,argv);\
  return 0;\
}

#define Coro_Main(argc,argv) Main();\
int coro_main(int argc, char *argv[])
