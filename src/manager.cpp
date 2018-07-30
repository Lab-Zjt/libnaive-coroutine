#include "manager.h"
#include "timer.h"
#include "context.h"
#include "scheduler.h"
#include <csignal>
#include <zconf.h>
#include <cstring>
#include <sys/epoll.h>

ContextManager::ContextManager(int index) : _sig(40 + index), _status(Status::creating), _max_event(16),
  _event_list(16) {
  _manager = new Context(1024 * 1024, &ContextManager::manage, this);
  _epfd = epoll_create(1024);
  if (_epfd == -1) {
    perror("epoll_create");
    exit(-1);
  }
}
void ContextManager::epoll() {
  auto ready_count = epoll_wait(_epfd, _event_list.data(), _max_event, 0);
  if (ready_count == _max_event) {
    _max_event *= 2;
    _event_list.reserve(unsigned(_max_event));
  }
  for (int i = 0; i < ready_count; ++i) {
    auto ctx = Context::pointer(_event_list[i].data.ptr);
    ctx->set_status(Context::Status::syscalling);
  }
}
void ContextManager::set_signal_handler() {
  signal(_sig, ContextManager::alarm);
}
void ContextManager::alarm(int sig) {
  auto mng = Scheduler::get_current_manager();
  if (sig == mng->signo()) {
    //should not swap context when it is syscalling or IOblocking(IOblocking is a special syscalling status)
    if (mng->_cur->status() == Context::Status::syscalling || mng->_cur->status() == Context::Status::IOblocking ||
        mng->_cur->status() == Context::Status::finished) {
      return;
    }
    mng->_cur->set_status(Context::Status::ready);
    mng->_manager->resume(mng->_cur);
  }
}
void ContextManager::start() {
  _timer = new Timer(0, 5000, _sig, this);
  _manager->resume(nullptr);
}
void ContextManager::fetch_from_queue() {
  std::lock_guard<std::mutex> lock(_mtx);
  for (auto &it:_queue) {
    _context_list.emplace_back(new Context(it));
  }
  _queue.clear();
}
void ContextManager::push_to_queue(std::function<void()> &&func) {
  std::lock_guard<std::mutex> lock(_mtx);
  _queue.emplace_back(func);
}
void ContextManager::manage() {
  while (true) {
    epoll();
    fetch_from_queue();
    if (_context_list.empty()) {
      //printf("%d switch to ready.\n", _sig - 40);
      _status = Status::ready;
      usleep(100);
    } else {
      //printf("%lu task in %d\n", _context_list.size(), _sig - 40);
      for (auto it = _context_list.begin(); it != _context_list.end();) {
        auto ctx = *it;
        if (ctx->status() == Context::Status::ready) {
          //context status would become ready when it creates or swap to manager by signal.
          ctx->set_status(Context::Status::running);
          //printf("%d switch to running.\n", _sig - 40);
          _status = Status::running;
          _cur = ctx;
          _timer->start();
          ctx->resume(_manager);
          _timer->stop();
          _cur = nullptr;
          _status = Status::signal_handling;
          ++it;
        } else if (ctx->status() == Context::Status::IOblocking) {
          //when context call a I/O block function, context status would become IOblocking
          ++it;
        } else if (ctx->status() == Context::Status::finished) {
          //when context function exit, context status would become finished.
          it = _context_list.erase(it);
        } else if (ctx->status() == Context::Status::syscalling) {
          //context status become syscalling when it call I/O block function, it would be added to epoll, when epoll return, context status would become syscalling.
          _status = Status::running;
          _cur = ctx;
          _timer->start();
          ctx->resume(_manager);
          _timer->stop();
          _cur = nullptr;
          _status = Status::signal_handling;
          ++it;
        }
      }
    }
  }
}