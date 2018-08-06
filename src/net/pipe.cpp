#include "net/pipe.h"
#include <fcntl.h>
#include <unistd.h>
#include <ucontext.h>
#include <sys/epoll.h>

namespace soranet {
  thread_local ucontext_t __helper;
  Pipe::Pipe() : _fd(-1), _isClosed(false) {}
  Pipe::Pipe(int fd) : _fd(fd), _isClosed(false) {
    if (fd > 0)fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
  }
  Pipe::~Pipe() {}
  std::string Pipe::read(size_t size) {
    auto buf = new char[size];
    auto count = ::read(_fd, buf, size);
    if (count < 0) {
      delete[]buf;
      return std::string{};
    }
    auto res = std::string(buf, count);
    delete[] buf;
    return res;
  }
  void Pipe::close() {
    if (!_isClosed) {
      ::close(_fd);
      _isClosed = true;
    }
  }
  void Pipe::write(const std::string &str) {
    if (_isClosed)
      return;
    ::write(_fd, str.c_str(), str.size());
  }
  bool Pipe::isBad() {return _fd < 0;}
  bool Pipe::isClosed() {return _isClosed;}
  int Pipe::fd() {return _fd;}
}