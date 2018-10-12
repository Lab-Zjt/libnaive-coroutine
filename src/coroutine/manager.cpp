#include "manager.h"
#include "timer.h"
#include "context.h"
#include "scheduler.h"
#include <csignal>
#include <zconf.h>
#include <cstring>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <hook/hook.h>

ContextManager::ContextManager(int index) : _status(Status::creating), _max_event(16), _event_list(16) {
  _manager = new Context(std::bind(&ContextManager::manage, this), 1024 * 1024);
  _epfd = epoll_create(1024);
  _evfd = eventfd(0, 0);
  if (_epfd == -1) {
    perror("epoll_create");
    exit(-1);
  }
  epoll_event ev{};
  ev.data.ptr = this;
  ev.events = EPOLLIN;
  if (-1 == epoll_ctl(_epfd, EPOLL_CTL_ADD, _evfd, &ev)) {
    perror("epoll_ctl");
  };
}
std::uint64_t ev_read_buf = 0;
std::uint64_t ev_write_buf = 1;
void ContextManager::epoll() {
  auto ready_count = epoll_wait(_epfd, _event_list.data(), _max_event, 0);
  if (ready_count == _max_event) {
    _max_event *= 2;
    _event_list.reserve(unsigned(_max_event));
  }
  for (int i = 0; i < ready_count; ++i) {
    if (_event_list[i].data.ptr == this) {
      ::read(_evfd, &ev_read_buf, sizeof(ev_read_buf));
      //printf("Read From Eventfd.\n");
    } else {
      static_cast<Context::pointer >(_event_list[i].data.ptr)->set_status(Context::Status::ready);
    }
  }
}
void ContextManager::no_task_epoll() {
  auto ready_count = epoll_wait(_epfd, _event_list.data(), _max_event, -1);
  for (int i = 0; i < ready_count; ++i) {
    if (_event_list[i].data.ptr == this) {
      ::read(_evfd, &ev_read_buf, sizeof(ev_read_buf));
    } else {
      static_cast<Context::pointer>(_event_list[i].data.ptr)->set_status(Context::Status::ready);
    }
  }
}
void ContextManager::wake_up() {
  if (_wake_up_mtx.try_lock()) {
    auto b = write(_evfd, &ev_write_buf, sizeof(ev_write_buf));
    _wake_up_mtx.unlock();
  }
}
void ContextManager::start() {
  _manager->resume(nullptr);
}
void ContextManager::fetch_from_queue() {
  std::vector<std::pair<std::function<void()>, size_t>> tmp;
  {
    std::lock_guard<std::mutex> lock(_mtx);
    for (auto &&it:_queue) {
      tmp.emplace_back(std::move(it));
    }
    _queue.clear();
  }
  for (auto &it:tmp) {
    _context_list.push_back(new Context(std::move(it.first), it.second));
  }
}
void ContextManager::push_to_queue(std::function<void()> &&func, size_t stack_size) {
  {
    std::lock_guard<std::mutex> lock(_mtx);
    _queue.emplace_back(std::make_pair(std::move(func), stack_size));
  }
  wake_up();
}
void ContextManager::manage() {
  while (true) {
    fetch_from_queue();
    if (_context_list.empty()) {
      _status = Status::ready;
      no_task_epoll();
    } else {
      epoll();
      //done : if all coroutine is I/O blocking, sleep 500us (it's totally... hard code).
      int done = 0;
      for (auto it = _context_list.begin(); it != _context_list.end();) {
        auto ctx = *it;
        if (ctx->status() == Context::Status::ready) {
          done++;
          //context status would become ready when it creates or swap to manager by signal.
          ctx->set_status(Context::Status::running);
          _status = Status::running;
          _cur = ctx;
          ctx->resume(_manager);
          _cur = nullptr;
          //_status = Status::signal_handling;
          ++it;
        } else if (ctx->status() == Context::Status::IOblocking) {
          //when context call a I/O block function, context status would become IOblocking
          ++it;
        } else if (ctx->status() == Context::Status::finished) {
          //when context function exit, context status would become finished.
          delete (*it);
          it = _context_list.erase(it);
        }
      }
      if (_context_list.empty()) {
        continue;
      }
      if (done == 0) {
        no_task_epoll();
      }
    }
  }
}