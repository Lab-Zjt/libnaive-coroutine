#include "net/connection.h"
#include "net/address.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

namespace soranet {
  Connection::~Connection() {
    if (!_isClosed)::close(_fd);
    delete _addr;
  }
  Connection::Connection(const std::string &ip, std::uint16_t port, int fd, bool isIpv6) : _addr(new Address(ip, port)),
    _isConnected(false) {
    _fd = fd;
    if (isIpv6) {
      _bad = true;
      return;
    }
    if (fd < 0) _bad = true;
    else fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
  }
  Connection::Connection(const ::sockaddr &addr, int fd) : _addr(new Address(addr)), _isConnected(false) {
    _fd = fd;
    if (_fd < 0)_bad = true;
    else fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
  }
  Connection::Connection(const soranet::Address &addr, int fd) : _addr(new Address(addr)), _isConnected(false) {
    _fd = fd;
    if (_fd < 0)_bad = true;
    else fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
  }
  Address &Connection::getAddress() {return *_addr;}
  std::shared_ptr<Connection> Dial(const std::string &ip, std::uint16_t port, bool isIpv6) {
    return Dial(Address(ip, port), isIpv6);
  }
  std::shared_ptr<Connection> Dial(const Address &addr, bool isIpv6) {
    int conn = ::socket(addr.getFamily(), SOCK_STREAM, 0);
    ::connect(conn, addr.getSockaddr(), sizeof(::sockaddr));
    return std::make_shared<Connection>(addr, conn);
  }
  std::shared_ptr<Connection> httpDial(const std::string &address, const std::string &port) {
    ::addrinfo hints{};
    ::addrinfo *res;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    hints.ai_addrlen = sizeof(sockaddr);
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;
    getaddrinfo(address.c_str(), port.c_str(), &hints, &res);
    int conn;
    for (auto ptr = res; ptr != nullptr; ptr = ptr->ai_next) {
      if (ptr->ai_family == AF_INET) {
        conn = socket(AF_INET, ptr->ai_socktype, ptr->ai_protocol);
        ::connect(conn, ptr->ai_addr, ptr->ai_addrlen);
        auto addr = *ptr->ai_addr;
        freeaddrinfo(res);
        return std::make_shared<Connection>(Address(addr), conn);
      }
    }
    freeaddrinfo(res);
    return nullptr;
  }
}