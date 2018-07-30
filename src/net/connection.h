#ifndef SORANET_CONNECTION_H
#define SORANET_CONNECTION_H

#include "pipe.h"
#include <memory>

struct sockaddr;
namespace soranet {
  
  class Address;
  
  class Connection : public Pipe {
    Address *_addr;
    bool _bad;
    bool _isConnected;
  public:
    Connection() = default;
    ~Connection() override;
    Connection(const std::string &ip, std::uint16_t port, int fd, bool isIpv6 = false);
    Connection(const sockaddr &addr, int fd);
    Connection(const Address &addr, int fd);
    Address &getAddress();
  };
  
  std::shared_ptr<Connection> Dial(const std::string &ip, std::uint16_t port, bool isIpv6 = false);
  std::shared_ptr<Connection> Dial(const Address &addr, bool isIpv6 = false);
  std::shared_ptr<Connection> httpDial(const std::string &address, std::uint16_t port = 80);
}

#endif
