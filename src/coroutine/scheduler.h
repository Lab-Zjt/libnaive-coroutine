#ifndef NAIVE_CORO_SCHEDULER_H
#define NAIVE_CORO_SCHEDULER_H

#include <thread>
#include <functional>
#include <atomic>
#include <mutex>
#include "manager.h"

class ContextManager;

class Context;

constexpr int max_thread = 5;

class Scheduler {
  typedef int(*main_t)(int, char *[]);
  ContextManager *_manager[max_thread];
  std::atomic<int> _index;
  bool _initializing;
public:
  Scheduler();
  static ContextManager *get_current_manager();
  void start(main_t cmain, int argc, char *argv[]);
  void init();
  bool empty();
  bool initializing() {return _initializing;}
  void push_func(std::function<void()> &&func,size_t stack_size);
};


extern Scheduler *_scheduler;

#endif
