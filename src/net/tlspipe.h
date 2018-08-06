#ifndef SORANET_TLSPIPE_H
#define SORANET_TLSPIPE_H

#include <net/pipe.h>
struct ssl_st;
struct ssl_method_st;
struct ssl_ctx_st;
typedef struct ssl_st SSL;
typedef struct ssl_method_st SSL_METHOD;
typedef struct ssl_ctx_st SSL_CTX;

namespace soranet {
  class TLSpipe : public Pipe {
  protected:
    SSL *_ssl;
    const SSL_METHOD *_method;
    SSL_CTX *_ctx;
  public:
    TLSpipe();
    explicit TLSpipe(int fd);
    ~TLSpipe() override;
    std::string tlsRead(size_t size);
    void tlsWrite(const std::string &str);
  };
}

#endif
