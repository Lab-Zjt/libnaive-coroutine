#include "hook.h"
#include "coroutine/scheduler.h"
#include "coroutine/manager.h"
#include "coroutine/context.h"
#include <dlfcn.h>
#include <ucontext.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/timerfd.h>

extern "C" {
void *libc = nullptr;
read_t origin_read = nullptr;
write_t origin_write = nullptr;
connect_t origin_connect = nullptr;
accept_t origin_accept = nullptr;
recv_t origin_recv = nullptr;
send_t origin_send = nullptr;
usleep_t origin_usleep = nullptr;
sleep_t origin_sleep = nullptr;
nanosleep_t origin_nanosleep = nullptr;
void hook_all() {
  libc = dlopen("libc.so.6", RTLD_LAZY);
  if (libc == nullptr) {
    _exit(-1);
  }
  origin_read = read_t(dlsym(libc, "read"));
  origin_write = write_t(dlsym(libc, "write"));
  origin_connect = connect_t(dlsym(libc, "connect"));
  origin_accept = accept_t(dlsym(libc, "accept"));
  origin_recv = recv_t(dlsym(libc, "recv"));
  origin_send = send_t(dlsym(libc, "send"));
  origin_nanosleep = nanosleep_t(dlsym(libc, "nanosleep"));
  origin_sleep = sleep_t(dlsym(libc, "sleep"));
  origin_usleep = usleep_t(dlsym(libc, "usleep"));
  if (origin_read == nullptr || origin_write == nullptr || origin_connect == nullptr || origin_accept == nullptr ||
      origin_recv == nullptr || origin_send == nullptr || origin_usleep == nullptr || origin_sleep == nullptr ||
      origin_nanosleep == nullptr) {
    exit(-1);
  }
}
ssize_t read(int fd, void *buf, size_t count) {
  if (origin_read == nullptr)hook_all();
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_read(fd, buf, count);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_read(fd, buf, count);
  }
  ssize_t res = 0;
  struct stat stat_buf{};
  fstat(fd, &stat_buf);
  if (!S_ISSOCK(stat_buf.st_mode)) {
    while (res != count) {
      auto realsize = origin_read(fd, buf + res, count - res);
      if (realsize == 0 || realsize == -1) break;
      res += realsize;
    }
  } else {
    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.ptr = cur;
    epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, fd, &ev);
    cur->set_status(Context::Status::IOblocking);
    mng->manager()->resume(cur);
    res = 0;
    while (res != count) {
      auto realsize = origin_read(fd, buf + res, count - res);
      if (realsize == 0 || realsize == -1) break;
      res += realsize;
    }
    epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, fd, nullptr);
  }
  //cur->set_status(Context::Status::running);
  return res;
}
ssize_t write(int fd, const void *buf, size_t count) {
  if (origin_write == nullptr)hook_all();
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_write(fd, buf, count);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_write(fd, buf, count);
  }
  struct stat stat_buf{};
  fstat(fd, &stat_buf);
  ssize_t res = 0;
  if (!S_ISSOCK(stat_buf.st_mode)) {
    while (res != count) {
      auto realsize = origin_write(fd, buf + res, count - res);
      if (realsize == 0 || realsize == -1) break;
      res += realsize;
    }
  } else {
    epoll_event ev{};
    ev.events = EPOLLOUT;
    ev.data.ptr = cur;
    epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, fd, &ev);
    cur->set_status(Context::Status::IOblocking);
    mng->manager()->resume(cur);
    res = 0;
    while (res != count) {
      auto realsize = ::origin_write(fd, buf + res, count - res);
      if (realsize == 0 || realsize == -1) break;
      res += realsize;
    }
    epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, fd, nullptr);
  }
  cur->set_status(Context::Status::running);
  return res;
}
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  if (origin_connect == nullptr)hook_all();
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_connect(sockfd, addr, addrlen);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_connect(sockfd, addr, addrlen);
  }
  int old_option = fcntl(sockfd, F_GETFL);
  fcntl(sockfd, F_SETFL, (unsigned) old_option | (unsigned) O_NONBLOCK);
  auto res = origin_connect(sockfd, addr, addrlen);
  if (errno == EINPROGRESS) {
    epoll_event ev{};
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.ptr = cur;
    epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, sockfd, &ev);
    cur->set_status(Context::Status::IOblocking);
    mng->manager()->resume(cur);
    res = origin_connect(sockfd, addr, addrlen);
    if (errno == EISCONN) {
      res = 0;
    }
    epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, sockfd, nullptr);
  }
  fcntl(sockfd, F_SETFL, old_option);
  cur->set_status(Context::Status::running);
  return res;
}
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  if (origin_accept == nullptr)hook_all();
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_accept(sockfd, addr, addrlen);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_accept(sockfd, addr, addrlen);
  }
  epoll_event ev{};
  ev.events = EPOLLIN;
  ev.data.ptr = cur;
  epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, sockfd, &ev);
  cur->set_status(Context::Status::IOblocking);
  mng->manager()->resume(cur);
  int fd = origin_accept(sockfd, addr, addrlen);
  epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, sockfd, nullptr);
  cur->set_status(Context::Status::running);
  return fd;
}
ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
  if (origin_recv == nullptr)hook_all();
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_recv(sockfd, buf, len, flags);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_recv(sockfd, buf, len, flags);
  }
  ssize_t res = 0;
  epoll_event ev{};
  ev.events = EPOLLIN;
  ev.data.ptr = cur;
  epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, sockfd, &ev);
  cur->set_status(Context::Status::IOblocking);
  mng->manager()->resume(cur);
  res = 0;
  while (res != len) {
    auto realsize = origin_recv(sockfd, buf + res, len - res, flags);
    if (realsize == 0 || realsize == -1) break;
    res += realsize;
  }
  epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, sockfd, nullptr);
  cur->set_status(Context::Status::running);
  return res;
}
ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
  if (origin_send == nullptr)hook_all();
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_send(sockfd, buf, len, flags);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_send(sockfd, buf, len, flags);
  }
  ssize_t res = 0;
  epoll_event ev{};
  ev.events = EPOLLOUT;
  ev.data.ptr = cur;
  epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, sockfd, &ev);
  cur->set_status(Context::Status::IOblocking);
  mng->manager()->resume(cur);
  res = 0;
  while (res != len) {
    auto realsize = origin_send(sockfd, buf + res, len - res, flags);
    if (realsize == 0 || realsize == -1) break;
    res += realsize;
  }
  epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, sockfd, nullptr);
  cur->set_status(Context::Status::running);
  return res;
}
int usleep(useconds_t usec) {
  if (usec == 0) return 0;
  if (origin_usleep == nullptr)hook_all();
  auto mng = Scheduler::get_current_manager();
  int fd = timerfd_create(CLOCK_REALTIME, 0);
  itimerspec spec{};
  spec.it_value.tv_sec = usec / 1000000;
  spec.it_value.tv_nsec = usec * 1000 % 1000000000;
  epoll_event ev{};
  ev.data.ptr = mng->current();
  ev.events = EPOLLIN;
  timerfd_settime(fd, 0, &spec, nullptr);
  epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, fd, &ev);
  mng->current()->set_status(Context::Status::IOblocking);
  mng->manager()->resume(mng->current());
  epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, fd, nullptr);
  close(fd);
  mng->current()->set_status(Context::Status::running);
  return 0;
}
unsigned int sleep(unsigned int seconds) {
  if (seconds == 0)return 0;
  if (origin_sleep == nullptr)hook_all();
  auto mng = Scheduler::get_current_manager();
  int fd = timerfd_create(CLOCK_REALTIME, 0);
  itimerspec spec{};
  spec.it_value.tv_sec = seconds;
  epoll_event ev{};
  ev.data.ptr = mng->current();
  ev.events = EPOLLIN;
  timerfd_settime(fd, 0, &spec, nullptr);
  epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, fd, &ev);
  mng->current()->set_status(Context::Status::IOblocking);
  mng->manager()->resume(mng->current());
  timerfd_gettime(fd, &spec);
  epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, fd, nullptr);
  close(fd);
  mng->current()->set_status(Context::Status::running);
  return 0;
}
int nanosleep(const struct timespec *req, struct timespec *rem) {
  if (req->tv_nsec == 0 && req->tv_sec == 0)return 0;
  if (origin_nanosleep == nullptr)hook_all();
  auto mng = Scheduler::get_current_manager();
  int fd = timerfd_create(CLOCK_REALTIME, 0);
  itimerspec spec{};
  spec.it_value = *req;
  epoll_event ev{};
  ev.data.ptr = mng->current();
  ev.events = EPOLLIN;
  timerfd_settime(fd, 0, &spec, nullptr);
  epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, fd, &ev);
  mng->current()->set_status(Context::Status::IOblocking);
  mng->manager()->resume(mng->current());
  epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, fd, nullptr);
  close(fd);
  mng->current()->set_status(Context::Status::running);
  return 0;
}
}
