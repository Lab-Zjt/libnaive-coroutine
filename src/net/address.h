#ifndef SORANET_ADDRESS_H
#define SORANET_ADDRESS_H

#include <string>

struct sockaddr;
namespace soranet {
  
  class Address {
  protected:
#define v6 ((::sockaddr_in6*)_addr)
#define v4 ((::sockaddr_in*)_addr)
    ::sockaddr* _addr;
    bool _bad;
    bool _ipv6;
  public:
    Address();
    Address(const Address& rhs);
    Address(const std::string &ip, std::uint16_t port, bool isIpv6 = false) noexcept;
    Address(const std::string &ip, const std::string &port, bool isIpv6 = false) noexcept;
    explicit Address(const ::sockaddr &addr, bool isIpv6 = false)noexcept;
    ~Address();
    std::string getIp();
    std::string getIp()const;
    std::uint16_t getPort();
    std::uint16_t getPort()const;
    //std::string getPortString();
    //std::string getPortString()const;
    ::sockaddr *getSockaddr();
    const ::sockaddr* getSockaddr()const;
    bool isIpv6();
    bool isIpv6()const;
    bool isBad();
    bool isBad()const;
    int getFamily();
    int getFamily()const;
    void setSockaddr(sockaddr *addr);
#undef v4
#undef v6
  };

#ifdef DEBUG
  namespace test {
    inline void address_test() {
      //Object
      Address addr("127.0.0.1", 1919);
      srlib::io::println(addr.getIp() + ":" + addr.getPortString());
      srlib::io::println(addr.getPort());
      srlib::io::println(addr.getFamily() == AF_INET);
      //Pointer
      auto paddr = new Address("fe80::b056:223:24e5:10d7", "3456", true);
      srlib::io::println(paddr->getIp() + ":" + paddr->getPortString());
      srlib::io::println(paddr->isBad());
      srlib::io::println(paddr->isIpv6());
      delete paddr;
    }
  }
#endif
}

#endif
