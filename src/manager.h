#ifndef NAIVECORO_MANAGER_H
#define NAIVECORO_MANAGER_H

#include <list>
#include <vector>
#include <atomic>
#include <functional>
#include <mutex>

class Context;

class Timer;

class ContextManager {
public:
  enum class Status {
    signal_handling, ready, running,creating
  };
  typedef ContextManager *pointer;
  typedef ContextManager &reference;
private:
  std::list<Context *> CtxQueue;
  Context *manager;
  Context *cur;
  std::vector<Context *> waiting_for_erase;
  Timer *timer;
  std::vector<std::function<void()>> queue;
public:
  int signo;
  Status status = Status::creating;
  bool avalible = true;
  ContextManager(int index);
  void push(Context *context) {
    std::lock_guard<std::mutex> lock(mtx);
    CtxQueue.push_back(context);
  }
  void push_to_queue(std::function<void()> &&func) {
    std::lock_guard<std::mutex> lock(mtx);
    queue.emplace_back(func);
  }
  void queue_to_ctx();
  std::mutex mtx;
  void start();
  std::atomic<bool> empty;
  pthread_t tid;
  bool running;
  void erase(Context *ctx);
  Context *main() {return manager;}
  Context *current() {return cur;}
  static void alarm(int signo);
  void manage();
  void setSignal();
};

#endif
