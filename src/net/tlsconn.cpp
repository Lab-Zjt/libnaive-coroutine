#include <net/tlsconn.h>
#include <net/address.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace soranet {
  TLSconnection::~TLSconnection() {
    if (!_isClosed)::close(_fd);
  }
  TLSconnection::TLSconnection(const std::string &ip, std::uint16_t port, int fd, bool isIpv6) : _addr(new Address(ip,
                                                                                                                   port)),
    _isConnected(false), _loop(nullptr) {
    _fd = fd;
    SSL_set_fd(_ssl, _fd);
    _bad = SSL_connect(_ssl) < 0;
    if (_bad)
      ERR_print_errors_fp(stdout);
  }
  TLSconnection::TLSconnection(const ::sockaddr &addr, int fd)
    : _addr(new Address(addr)), _isConnected(false), _loop(nullptr) {
    _fd = fd;
    if (_fd < 0)_bad = true;
    SSL_set_fd(_ssl, _fd);
    _bad = SSL_connect(_ssl) < 0;
    if (_bad)
      ERR_print_errors_fp(stdout);
  }
  TLSconnection::TLSconnection(const soranet::Address &addr, int fd)
    : _addr(new Address(addr)), _isConnected(false), _loop(nullptr) {
    _fd = fd;
    if (_fd < 0)_bad = true;
    SSL_set_fd(_ssl, _fd);
    _bad = SSL_connect(_ssl) < 0;
    if (_bad)
      ERR_print_errors_fp(stdout);
  }
  Address &TLSconnection::getAddress() {return *_addr;}
  void TLSconnection::setEventLoop(soranet::EventLoop *loop) {_loop = loop;}
  std::shared_ptr<TLSconnection> tlsDial(const std::string &ip, std::uint16_t port, bool isIpv6) {
    return tlsDial(Address(ip, port), isIpv6);
  }
  std::shared_ptr<TLSconnection> tlsDial(const Address &addr, bool isIpv6) {
    int conn = ::socket(addr.getFamily(), SOCK_STREAM, 0);
    ::connect(conn, addr.getSockaddr(), sizeof(::sockaddr));
    return std::make_shared<TLSconnection>(addr, conn);
  }
  std::shared_ptr<TLSconnection> httpsDial(const std::string &address, const std::string &port) {
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
        return std::make_shared<TLSconnection>(Address(addr), conn);
      }
    }
    freeaddrinfo(res);
    return nullptr;
  }
  void SSLinitialize() {
    OpenSSL_add_all_algorithms();
    SSL_library_init();
    SSL_load_error_strings();
    printf("SSL Initialized!\n");
  }
}