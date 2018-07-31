#ifndef NAIVECORO_MANAGER_H
#define NAIVECORO_MANAGER_H

#include <list>
#include <vector>
#include <atomic>
#include <functional>
#include <mutex>

class Context;

class Timer;

struct epoll_event;

class ContextManager {
public:
  enum class Status {
    signal_handling, ready, running, creating
  };
  typedef ContextManager *pointer;
  typedef ContextManager &reference;
private:
  std::list<Context *> _context_list;
  Context *_manager;
  Context *_cur;
  std::mutex _mtx;
  std::mutex _wake_up_mtx;
  Timer *_timer;
  std::vector<std::function<void()>> _queue;
  int _sig;
  int _epfd;
  pthread_t _tid;
  Status _status;
  std::vector<epoll_event> _event_list;
  int _max_event;
  int _evfd;
public:
  explicit ContextManager(int index);
  inline int signo() {return _sig;}
  inline pthread_t tid() {return _tid;}
  inline void set_tid(pthread_t t) {_tid = t;}
  inline Status status() {return _status;}
  inline void set_status(Status st) {_status = st;}
  inline Context *manager() {return _manager;}
  inline Context *current() {return _cur;}
  inline int epfd() {return _epfd;}
  void push_to_queue(std::function<void()> &&func);
  void fetch_from_queue();
  void start();
  void manage();
  void set_signal_handler();
  void epoll();
  void no_task_epoll();
  void wake_up();
  static void alarm(int sig);
};

#endif