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

extern "C" {
void *libc = nullptr;
read_t origin_read = nullptr;
write_t origin_write = nullptr;
open_t origin_open = nullptr;
close_t origin_close = nullptr;
socket_t origin_socket = nullptr;
connect_t origin_connect = nullptr;
bind_t origin_bind = nullptr;
listen_t origin_listen = nullptr;
accept_t origin_accept = nullptr;
printf_t origin_printf = nullptr;
malloc_t origin_malloc = nullptr;
free_t origin_free = nullptr;
void hook_all() {
  libc = dlopen("libc.so.6", RTLD_LAZY);
  if (libc == nullptr) {
    _exit(-1);
  }
  origin_read = read_t(dlsym(libc, "read"));
  origin_write = write_t(dlsym(libc, "write"));
  origin_open = open_t(dlsym(libc, "open"));
  origin_close = close_t(dlsym(libc, "close"));
  origin_socket = socket_t(dlsym(libc, "socket"));
  origin_connect = connect_t(dlsym(libc, "connect"));
  origin_bind = bind_t(dlsym(libc, "bind"));
  origin_listen = listen_t(dlsym(libc, "listen"));
  origin_accept = accept_t(dlsym(libc, "accept"));
  origin_printf = printf_t(dlsym(libc, "printf"));
  origin_free = free_t(dlsym(libc, "free"));
  if (origin_read == nullptr || origin_write == nullptr || origin_open == nullptr || origin_close == nullptr ||
      origin_socket == nullptr || origin_connect == nullptr || origin_bind == nullptr || origin_listen == nullptr ||
      origin_accept == nullptr || origin_printf == nullptr || origin_free == nullptr) {
    exit(-1);
  }
}
ssize_t read(int fd, void *buf, size_t count) {
  printf("Hook Read Call!\n");
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_read(fd, buf, count);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_read(fd, buf, count);
  }
  auto save = cur->status();
  cur->set_status(Context::Status::syscalling);
  ssize_t res = 0;
  struct stat stat_buf{};
  fstat(fd, &stat_buf);
  if (S_ISREG(stat_buf.st_mode)) {
    while (res != count) {
      auto realsize = origin_read(fd, buf + res, count - res);
      if (realsize == 0 | realsize == -1) break;
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
      if (realsize == 0 | realsize == -1) break;
      res += realsize;
    }
    epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, fd, nullptr);
  }
  cur->set_status(save);
  return res;
}
ssize_t write(int fd, const void *buf, size_t count) {
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_write(fd, buf, count);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_write(fd, buf, count);
  }
  auto save = cur->status();
  cur->set_status(Context::Status::syscalling);
  struct stat stat_buf{};
  fstat(fd, &stat_buf);
  ssize_t res = 0;
  if (S_ISREG(stat_buf.st_mode)) {
    while (res != count) {
      auto realsize = origin_write(fd, buf + res, count - res);
      if (realsize == 0 | realsize == -1) break;
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
      if (realsize == 0 | realsize == -1) break;
      res += realsize;
    }
    epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, fd, nullptr);
  }
  cur->set_status(save);
  return res;
}
int open(const char *pathname, int flags, ...) {
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    if ((unsigned) flags & (unsigned) O_CREAT) {
      va_list vl;
      va_start(vl, flags);
      mode_t mode = va_arg(vl, mode_t);
      va_end(vl);
      return origin_open(pathname, flags, mode);
    }
    return origin_open(pathname, flags);
  } else {
    auto cur = mng->current();
    if (cur == nullptr) {
      if ((unsigned) flags & (unsigned) O_CREAT) {
        va_list vl;
        va_start(vl, flags);
        mode_t mode = va_arg(vl, mode_t);
        va_end(vl);
        return origin_open(pathname, flags, mode);
      }
      return origin_open(pathname, flags);
    } else {
      auto save = cur->status();
      cur->set_status(Context::Status::syscalling);
      int fd;
      if ((unsigned) flags & (unsigned) O_CREAT) {
        va_list vl;
        va_start(vl, flags);
        mode_t mode = va_arg(vl, mode_t);
        va_end(vl);
        fd = origin_open(pathname, flags, mode);
      } else {
        fd = origin_open(pathname, flags);
      }
      cur->set_status(save);
      return fd;
    }
  }
}
int close(int fd) {
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_close(fd);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_close(fd);
  }
  auto save = cur->status();
  cur->set_status(Context::Status::syscalling);
  auto res = origin_close(fd);
  cur->set_status(save);
  return res;
}
int socket(int domain, int type, int protocol) {
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_socket(domain, type, protocol);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_socket(domain, type, protocol);
  }
  auto save = cur->status();
  cur->set_status(Context::Status::syscalling);
  auto res = origin_socket(domain, type, protocol);
  cur->set_status(save);
  return res;
}
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_connect(sockfd, addr, addrlen);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_connect(sockfd, addr, addrlen);
  }
  auto save = cur->status();
  cur->set_status(Context::Status::syscalling);
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
    epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, sockfd, nullptr);
  }
  fcntl(sockfd, F_SETFL, old_option);
  cur->set_status(save);
  return res;
}
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_bind(sockfd, addr, addrlen);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_bind(sockfd, addr, addrlen);
  }
  auto save = cur->status();
  cur->set_status(Context::Status::syscalling);
  auto res = origin_bind(sockfd, addr, addrlen);
  cur->set_status(save);
  return res;
}
int listen(int sockfd, int backlog) {
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_listen(sockfd, backlog);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_listen(sockfd, backlog);
  }
  auto save = cur->status();
  cur->set_status(Context::Status::syscalling);
  auto res = origin_listen(sockfd, backlog);
  cur->set_status(save);
  return res;
}
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_accept(sockfd, addr, addrlen);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_accept(sockfd, addr, addrlen);
  }
  auto save = cur->status();
  cur->set_status(Context::Status::syscalling);
  epoll_event ev{};
  ev.events = EPOLLIN;
  ev.data.ptr = cur;
  epoll_ctl(mng->epfd(), EPOLL_CTL_ADD, sockfd, &ev);
  cur->set_status(Context::Status::IOblocking);
  mng->manager()->resume(cur);
  int fd = origin_accept(sockfd, addr, addrlen);
  epoll_ctl(mng->epfd(), EPOLL_CTL_DEL, sockfd, nullptr);
  cur->set_status(save);
  return fd;
}
void hook_malloc() {
  auto libc = dlopen("libc.so.6", RTLD_LAZY);
  if (libc == nullptr) {
    exit(1);
  }
  origin_malloc = malloc_t(dlsym(libc, "malloc"));
  dlclose(libc);
}
char malloc_buf[1024];
int buf_pos = 0;
void *malloc(size_t size) {
  static bool is_hooking = false;
  if (origin_malloc == nullptr) {
    if (!is_hooking) {
      is_hooking = true;
      hook_malloc();
      is_hooking = false;
    } else {
      if (buf_pos + size < 1024) {
        void *ptr = malloc_buf + buf_pos;
        buf_pos += size;
        return ptr;
      } else {
        exit(2);
      }
    }
  }
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    return origin_malloc(size);
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    return origin_malloc(size);
  }
  cur->set_status(Context::Status::syscalling);
  auto res = origin_malloc(size);
  cur->set_status(Context::Status::running);
  return res;
}
void free(void *ptr) {
  auto mng = Scheduler::get_current_manager();
  if (mng == nullptr) {
    origin_free(ptr);
    return;
  }
  auto cur = mng->current();
  if (cur == nullptr) {
    origin_free(ptr);
    return;
  }
  cur->set_status(Context::Status::syscalling);
  origin_free(ptr);
  cur->set_status(Context::Status::running);
}
}
