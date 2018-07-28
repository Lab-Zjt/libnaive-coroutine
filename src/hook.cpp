#include "hook.h"
#include "scheduler.h"
#include "manager.h"
#include "context.h"
#include <dlfcn.h>
#include <ucontext.h>
#include <sys/epoll.h>

extern "C" {
void *libc = nullptr;
read_t origin_read = nullptr;
write_t origin_write = nullptr;
close_t origin_close = nullptr;
accept_t origin_accept = nullptr;
printf_t origin_printf = nullptr;
static int counter = 0;
void hook_all() {
  libc = dlopen("libc.so.6", RTLD_LAZY);
  if (libc == nullptr) {
    _exit(-1);
  }
  origin_read = read_t(dlsym(libc, "read"));
  origin_write = write_t(dlsym(libc, "write"));
  origin_close = close_t(dlsym(libc, "close"));
  origin_accept = accept_t(dlsym(libc, "accept"));
  origin_printf = printf_t(dlsym(libc, "printf"));
  if (origin_read == nullptr || origin_write == nullptr || origin_accept == nullptr || origin_close == nullptr ||
      origin_printf == nullptr) {
    _exit(-1);
  }
}
int printf(const char *format, ...) {
  auto mng = Scheduler::getCurrentManager();
  int res;
  if (mng == nullptr) {
    va_list vl;
    va_start(vl, format);
    res = vfprintf(stdout, format, vl);
    va_end(vl);
  } else {
    auto cur = mng->current();
    if (cur == nullptr) {
      va_list vl;
      va_start(vl, format);
      res = vfprintf(stdout, format, vl);
      va_end(vl);
    } else {
      cur->status = Context::Status::syscalling;
      va_list vl;
      va_start(vl, format);
      res = vfprintf(stdout, format, vl);
      va_end(vl);
      cur->status = Context::Status::running;
    }
  }
  return res;
}
ssize_t read(int fd, void *buf, size_t count) {
  auto mng = Scheduler::getCurrentManager();
  mng->current()->status = Context::Status::syscalling;
  auto res = origin_read(fd, buf, count);
  mng->current()->status = Context::Status::running;
  return res;
}
ssize_t write(int fd, const void *buf, size_t count) {
  auto mng = Scheduler::getCurrentManager();
  mng->current()->status = Context::Status::syscalling;
  auto res = origin_write(fd, buf, count);
  mng->current()->status = Context::Status::running;
  return res;
}
int close(int fd) {
  /*auto &ctx = soranet::FdCtx[fd];
  if (ctx == nullptr) {
    return origin_close(fd);
  }
  auto loop = soranet::getCurrentLoop();
  loop->_stack_list.push_back((char *) ctx->uc_stack.ss_sp);
  loop->_ucp_list.push_back(ctx);
  ctx = nullptr;*/
  return origin_close(fd);
}
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  int fd = origin_accept(sockfd, addr, addrlen);
  /*if (fd < 0) {
    return fd;
  }
  auto &ctx = soranet::FdCtx[fd];
  ctx = new ucontext_t;
  getcontext(ctx);
  ctx->uc_stack.ss_sp = new char[8192];
  ctx->uc_stack.ss_size = 8192;*/
  return fd;
}
}
