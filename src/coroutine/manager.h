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
    ready, running, creating
  };
  typedef ContextManager *pointer;
  typedef ContextManager &reference;
private:
  std::list<Context *> _context_list;
  Context *_manager;
  Context *_cur;
  std::mutex _mtx;
  std::mutex _wake_up_mtx;
  std::vector<std::pair<std::function<void()>, size_t >> _queue;
  int _epfd;
  pthread_t _tid;
  Status _status;
  int _max_event;
  std::vector<epoll_event> _event_list;
  int _evfd;
public:
  explicit ContextManager(int index);
  inline pthread_t tid() {return _tid;}
  inline void set_tid(pthread_t t) {_tid = t;}
  inline Status status() {return _status;}
  inline void set_status(Status st) {_status = st;}
  inline Context *manager() {return _manager;}
  inline Context *current() {return _cur;}
  inline int epfd() {return _epfd;}
  void push_to_queue(std::function<void()> &&func, size_t size);
  void fetch_from_queue();
  void start();
  void manage();
  void epoll();
  void no_task_epoll();
  void wake_up();
};

#endif
