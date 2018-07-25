#ifndef NAIVE_CORO_SCHEDULER_H
#define NAIVE_CORO_SCHEDULER_H

#include <thread>
#include <functional>
#include <atomic>
#include <mutex>

class ContextManager;

class Context;

constexpr int maxThread = 7;

class Scheduler {
  typedef int(*main_t)(int, char *[]);
  ContextManager *manager[maxThread];
  pthread_t id[maxThread];
  std::atomic<int> index;
  bool initializing = true;
public:
  std::atomic<int> task_count;
  Scheduler();
  std::mutex mtx;
  void push_task(Context *ctx);
  void push_main(main_t cmain, int argc, char *argv[]);
  static ContextManager *getCurrentManager();
  void start(main_t cmain, int argc, char *argv[]);
  void init();
  bool empty();
  void push_func(std::function<void()>&& func);
};

extern Scheduler *_scheduler;

#endif
