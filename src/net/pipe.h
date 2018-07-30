#ifndef SORANET_FD_H
#define SORANET_FD_H

#include <cstdint>
#include <string>

namespace soranet {
  class Pipe {
  protected:
    int _fd;
    bool _isClosed;
  public:
    Pipe();
    explicit Pipe(int fd);
    virtual ~Pipe();
    std::string read(size_t size);
    void write(const std::string &str);
    void close();
    bool isBad();
    bool isClosed();
    int fd();
  };
}

#endif
