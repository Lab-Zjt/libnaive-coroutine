#include "tls_connection.h"
#include "address.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace srlib {
  namespace net {
    void SSLInit() {
      OpenSSL_add_all_algorithms();
      SSL_library_init();
      SSL_load_error_strings();
    }
    TlsConnection::TlsConnection(const Address &addr, bool connect) : TcpConnection(addr, connect) {
      _method = TLSv1_2_client_method();
      _ctx = SSL_CTX_new(_method);
      _ssl = SSL_new(_ctx);
      SSL_set_fd(_ssl, fd());
      if (SSL_connect(_ssl) <= 0) {
        ERR_print_errors_fp(stdout);
      }
    }
    TlsConnection::~TlsConnection() {
      SSL_free(_ssl);
      SSL_CTX_free(_ctx);
      Close();
    }
    TlsConnection::TlsConnection(srlib::net::TlsConnection &&rhs)noexcept {
      _ssl = rhs._ssl;
      _ctx = rhs._ctx;
      _method = rhs._method;
      rhs._ssl = nullptr;
      rhs._ctx = nullptr;
      rhs._method = nullptr;
    }
    TlsConnection &TlsConnection::operator=(srlib::net::TlsConnection &&rhs)noexcept {
      _ssl = rhs._ssl;
      _ctx = rhs._ctx;
      _method = rhs._method;
      rhs._ssl = nullptr;
      rhs._ctx = nullptr;
      rhs._method = nullptr;
      return *this;
    }
    TlsConnection::TlsConnection(const String &ip, uint16_t port, bool connect) : TlsConnection(Address(ip, port),
                                                                                                connect) {}
    TlsConnection::TlsConnection(const String &ip, const String &port, bool connect) : TlsConnection(Address(ip, port),
                                                                                                     connect) {}
    String TlsConnection::TlsRead(size_t size) const {
      auto buf = new char[size]{};
      auto count = 0;
      while (count != size) {
        auto realsize = SSL_read(_ssl, buf + count, size - count);
        // TODO : handle read failed
        if (realsize == 0 || realsize == -1) break;
        count += realsize;
        {
          printf("%d/%lu\r", count, size);
          fflush(stdout);
        }
        // FIXME : usually SSL_read() return 16384 and if return value < 16384 mean all content was read, but fxxk www.baidu.com will not return 16384. Now it cannot handle non-block socket.
        if (realsize < 16384)break;
      }
      if (count < 0) {
        delete[]buf;
        return String();
      }
      auto res = String(buf, count);
      delete[]buf;
      return res;
    }
    ssize_t TlsConnection::TlsWrite(const srlib::String &msg) const {
      auto cstr = msg.c_str();
      auto size = msg.size();
      auto count = 0;
      while (count != size) {
        auto realsize = SSL_write(_ssl, cstr + count, size - count);
        //TODO : handle write failed
        if (realsize < 16384)return count;
        if (realsize == -1)return count;
        if (realsize == 0)return count;
        count += realsize;
      }
    }
  }
}