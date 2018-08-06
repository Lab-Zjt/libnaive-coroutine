#ifndef SORANET_TLSCONN_H
#define SORANET_TLSCONN_H

#include "tlspipe.h"
#include <memory>

struct sockaddr;
namespace soranet {
  class EventLoop;
  
  class Address;
  
  class TLSconnection : public TLSpipe {
    Address *_addr;
    bool _bad;
    bool _isConnected;
    EventLoop *_loop;
  public:
    TLSconnection() = default;
    ~TLSconnection();
    TLSconnection(const std::string &ip, std::uint16_t port, int fd, bool isIpv6 = false);
    TLSconnection(const sockaddr &addr, int fd);
    TLSconnection(const Address &addr, int fd);
    Address &getAddress();
    void setEventLoop(EventLoop *loop);
  };
  
  void SSLinitialize();
  std::shared_ptr<TLSconnection> tlsDial(const std::string &ip, std::uint16_t port, bool isIpv6 = false);
  std::shared_ptr<TLSconnection> tlsDial(const Address &addr, bool isIpv6 = false);
  std::shared_ptr<TLSconnection> httpsDial(const std::string &address, const std::string &port = "443");
}

#endif
