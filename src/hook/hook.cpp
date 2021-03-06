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
#define HOOK_CHECK(name)\
if (origin_##name == nullptr) {\
  if (libc == nullptr) {\
    libc = dlopen("libc.so.6", RTLD_LAZY);\
    if (libc == nullptr) {\
        exit(-1);\
    }\
  }\
  origin_##name = (decltype(origin_##name))(dlsym(libc, #name));\
  if (origin_##name == nullptr) {\
    exit(-1);\
  }\
}
ssize_t read(int fd, void *buf, size_t count) {
  HOOK_CHECK(read);
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
  // If fd is not a socket, use origin_read.
  if (!S_ISSOCK(stat_buf.st_mode)) {
    res = origin_read(fd, buf, count);
  } else {
    // If fd is a socket.
    // - Try to read the socket.
    res = origin_read(fd, buf, count);
    // - If socket can be read or error occurred, return.
    if (res != -1 || errno != EAGAIN) {return res;}
    // - Else add fd to epoll and yield, wait epoll return.
    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.ptr = cur;
    epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, fd, &ev);
    cur->set_status(Context::Status::IOblocking);
    mng->manager()->resume(cur);
    res = read(fd, buf, count);
    epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, fd, nullptr);
  }
  return res;
}
ssize_t write(int fd, const void *buf, size_t count) {
  HOOK_CHECK(write);
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
  // If fd is not a socket, use origin_read.
  if (!S_ISSOCK(stat_buf.st_mode)) {
    res = origin_write(fd, buf, count);
  } else {
    // If fd is a socket.
    // - Try to read the socket.
    res = origin_write(fd, buf, count);
    // - If socket can be read or error occurred, return.
    if (res != -1 || errno != EAGAIN)return res;
    // - Else add fd to epoll and yield, wait epoll return.
    epoll_event ev{};
    ev.events = EPOLLOUT;
    ev.data.ptr = cur;
    epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, fd, &ev);
    cur->set_status(Context::Status::IOblocking);
    mng->manager()->resume(cur);
    res = origin_write(fd, buf, count);
    epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, fd, nullptr);
  }
  return res;
}
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  HOOK_CHECK(connect);
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
  HOOK_CHECK(accept);
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
  HOOK_CHECK(recv);
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
  HOOK_CHECK(send);
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
  HOOK_CHECK(usleep);
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
  HOOK_CHECK(sleep);
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
  HOOK_CHECK(nanosleep);
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
