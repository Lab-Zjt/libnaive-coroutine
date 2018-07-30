#ifndef NAIVE_CORO_SCHEDULER_H
#define NAIVE_CORO_SCHEDULER_H

#include <thread>
#include <functional>
#include <atomic>
#include <mutex>

class ContextManager;

class Context;

constexpr int max_thread = 2;

class Scheduler {
  typedef int(*main_t)(int, char *[]);
  ContextManager *_manager[max_thread];
  pthread_t _id[max_thread];
  std::atomic<int> _index;
  bool _initializing;
public:
  Scheduler();
  //std::mutex mtx;
  static ContextManager *get_current_manager();
  void start(main_t cmain, int argc, char *argv[]);
  void init();
  bool empty();
  bool initializing() {return _initializing;}
  void push_func(std::function<void()> &&func);
};

extern Scheduler *_scheduler;

#endif
