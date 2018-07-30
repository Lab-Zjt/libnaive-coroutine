#ifndef SORANET_LISTENER_H
#define SORANET_LISTENER_H

#include "pipe.h"

namespace soranet {
  class Connection;
  
  class Address;
  
  class Listener : public Pipe {
  protected:
    Address *_addr;
  public:
    Listener();
    ~Listener() override;
    Listener(const std::string &ip, std::uint16_t port, bool isIpv6 = false);
    void listen(int backlog);
    Connection *accpet();
    Address &getAddress();
  };
  
  Listener Listen(const std::string &ip, std::uint16_t port, int backlog = 10);
}

#endif
