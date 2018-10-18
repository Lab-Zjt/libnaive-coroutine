#ifndef CORONET_GO_H
#define CORONET_GO_H

#include "scheduler.h"
#include "../hook/hook.h"

int coro_main(int argc, char *argv[]);
#if SORA_USE_SSL != 0

#include "../net/tls_connection.h"

#define Main() \
int main(int argc,char* argv[]){\
  srlib::net::SSLInit();\
  hook_all();\
  _scheduler = new Scheduler;\
  _scheduler->start(coro_main,argc,argv);\
  return 0;\
}
#else
#define Main() \
int main(int argc,char* argv[]){\
  hook_all();\
  _scheduler = new Scheduler;\
  _scheduler->start(coro_main,argc,argv);\
  return 0;\
}
#endif

#define Coro_Main(argc, argv) Main();\
int coro_main(int argc, char *argv[])

template<size_t STACK_SIZE = NORMAL_STACK_SIZE, typename Func, typename ...ARGS>
void go(Func &&func, ARGS &&...args) {
  _scheduler->push_func(std::bind(func, args...), STACK_SIZE);
};
#endif