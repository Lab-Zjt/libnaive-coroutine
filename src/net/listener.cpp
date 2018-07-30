#include "net/listener.h"
#include "net/connection.h"
#include "net/address.h"
#include <arpa/inet.h>
#include <netdb.h>

namespace soranet {
  Listener::Listener() : _addr(new Address()) {}
  Listener::~Listener(){
    delete _addr;
  }
  Listener::Listener(const std::string &ip, std::uint16_t port, bool isIpv6) {
    _addr = new Address(ip, port);
    if (isIpv6) {
      close();
      _fd = -1;
      return;
    }
    _fd = ::socket(_addr->getFamily(), SOCK_STREAM, 0);
  }
  void Listener::listen(int backlog) {
    ::bind(_fd, _addr->getSockaddr(), sizeof(::sockaddr)) == -1 ? close(), _fd = -1 : _fd;
    perror("bind");
    ::listen(_fd, backlog) == -1 ? close(), _fd = -1 : _fd;
  }
  Connection *Listener::accpet() {
    ::sockaddr addr{};
    ::socklen_t len = sizeof(addr);
    int fd = ::accept(_fd, &addr, &len);
    printf("Accept %d From %d\n", fd, ((sockaddr_in *) (&addr))->sin_port);
    if (fd < 0) {
      return nullptr;
    }
    return new Connection(addr, fd);
  }
  Address &Listener::getAddress() {return *_addr;}
  Listener Listen(const std::string &ip, std::uint16_t port, int backlog) {
    Listener l(ip, port);
    l.listen(backlog);
    return l;
  }
}