#include <net/tlspipe.h>
#include <openssl/ssl.h>
#include <iostream>

namespace soranet {
  TLSpipe::TLSpipe() {
    _method = TLS_client_method();
    _ctx = SSL_CTX_new(_method);
    _ssl = SSL_new(_ctx);
  }
  TLSpipe::TLSpipe(int fd) : TLSpipe() {
    _fd = fd;
    SSL_set_fd(_ssl, _fd);
  }
  TLSpipe::~TLSpipe() {
    SSL_free(_ssl);
    SSL_CTX_free(_ctx);
  }
  inline void rlog(ssize_t size) {std::clog << "Read " << size << " bytes." << std::endl;}
  inline void wlog(ssize_t size) {std::clog << "Write " << size << " bytes." << std::endl;}
  std::string TLSpipe::tlsRead(size_t size) {
    auto buf = new char[size]{};
    auto count = 0;
    while (count != size) {
      auto realsize = SSL_read(_ssl, buf + count, size - count);
      rlog(realsize);
      if (realsize == 5)break;
      auto err = SSL_get_error(_ssl, realsize);
      if (err == SSL_ERROR_NONE || (err == SSL_ERROR_SYSCALL && errno == 0)) {
        count += realsize;
      } else {
        break;
      }
      // TODO : handle read failed
      if (realsize == 0 || realsize == -1) break;
      printf("%d/%lu\r", count, size);
      fflush(stdout);
    }
    auto res = std::string(buf, count);
    delete buf;
    return res;
  }
  void TLSpipe::tlsWrite(const std::string &str) {
    auto cstr = str.c_str();
    auto size = str.size();
    auto count = 0;
    while (count != size) {
      auto realsize = SSL_write(_ssl, cstr + count, size - count);
      wlog(realsize);
      //TODO : handle write failed
      if (realsize <= 16384)return;
      if (realsize == -1)return;
      if (realsize == 0)return;
      count += realsize;
    }
  }
}