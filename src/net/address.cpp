#include "net/address.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <memory.h>

namespace srlib {
  template<typename INTTYPE>
  inline INTTYPE toBigEndian(INTTYPE i) {
#if LITTLE_ENDIAN == BYTE_ORDER
    INTTYPE res = 0;
    for (int it = 0; it < sizeof(INTTYPE); it++) {
      res <<= 8;
      res |= (i & 0xff);
      i >>= 8;
    }
    return res;
#endif
    return i;
  }
  template<typename INTTYPE>
  inline INTTYPE toLittleEndian(INTTYPE i) {
#if BIG_ENDIAN == BYTE_ORDER
    INTTYPE res = 0;
    for (int it = 0; it < sizeof(INTTYPE); it++) {
      res <<= 8;
      res |= (i & 0xff);
      i >>= 8;
    }
    return res;
#endif
    return i;
  }
}
namespace soranet {
#define v6 ((::sockaddr_in6*)(_addr))
#define v4 ((::sockaddr_in*)(_addr))
  Address::Address() : _addr(new sockaddr),_bad(true),  _ipv6(false) {}
  Address::Address(const std::string &ip, std::uint16_t port, bool isIpv6)noexcept {
    _addr = new sockaddr;
    memset(_addr, 0, sizeof(sockaddr));
    _ipv6 = isIpv6;
    if (isIpv6) {
      v6->sin6_port = srlib::toBigEndian(port);
      v6->sin6_family = AF_INET6;
      auto status = ::inet_pton(AF_INET6, ip.c_str(), &v6->sin6_addr);
      if (status < 1)_bad = true;
    } else {
      v4->sin_port = srlib::toBigEndian(port);
      v4->sin_family = AF_INET;
      auto status = ::inet_pton(AF_INET, ip.c_str(), &(v4->sin_addr));
      if (status < 1)_bad = true;
    }
  }
  Address::Address(const std::string &ip, const std::string &port, bool isIpv6)noexcept
    : Address(ip, std::atoi(port.c_str()), isIpv6) {}
  Address::Address(const ::sockaddr &addr, bool isIpv6)noexcept : _bad(false),_ipv6(isIpv6) {
    _addr = new ::sockaddr;
    memmove(_addr, &addr, sizeof(addr));
  }
  Address::Address(const soranet::Address &rhs) {
    _addr = new ::sockaddr;
    memmove(_addr, rhs._addr, sizeof(::sockaddr));
    _ipv6 = rhs._ipv6;
    _bad = rhs._bad;
  }
  Address::~Address() {
    delete _addr;
  }
  std::string Address::getIp() {
    char buf[64];
    _ipv6 ? ::inet_ntop(AF_INET6, &v6->sin6_addr, buf, 64) : ::inet_ntop(AF_INET, &v4->sin_addr, buf, 64);
    return std::string(buf);
  }
  std::string Address::getIp() const {return const_cast<Address *>(this)->getIp();}
  std::uint16_t Address::getPort() {return srlib::toBigEndian(v4->sin_port);}
  std::uint16_t Address::getPort() const {return const_cast<Address *>(this)->getPort();}
  //std::string Address::getPortString() {return srlib::itoa(getPort());}
  //std::string Address::getPortString() const {return srlib::itoa(getPort());}
  ::sockaddr *Address::getSockaddr() {return _addr;}
  const ::sockaddr *Address::getSockaddr() const {return _addr;}
  bool Address::isIpv6() const {return _ipv6;}
  bool Address::isBad() const {return _bad;}
  int Address::getFamily() const {return _ipv6 ? AF_INET6 : AF_INET;}
  bool Address::isIpv6() {return _ipv6;}
  bool Address::isBad() {return _bad;}
  int Address::getFamily() {return _ipv6 ? AF_INET6 : AF_INET;}
  void Address::setSockaddr(sockaddr *addr) {memmove(&_addr, addr, sizeof(sockaddr));}
#undef v4
#undef v6
}