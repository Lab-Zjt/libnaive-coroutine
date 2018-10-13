#include <utility/io.h>
#include "utility/exstring.h"
#include "connector.h"
#include "connection.h"
#include "address.h"
#include "tls_connection.h"
#include "parse.h"

namespace srlib {
  namespace net {
    String httpGet(const String &url, const String &append, std::uint64_t maxSize) {
      auto partition = url.find('/');
      auto domain = partition == std::string::npos ? url : url(0, partition);
      auto filename = partition == std::string::npos ? "/" : url(partition, url.size());
      auto conn = Dial(Address(domain, 80));
      String request = "GET " + filename + " HTTP/1.1\r\n""Host: " + domain +
                       "\r\n""User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:60.0) Gecko/20100101 Firefox/60.0\r\n"
                       "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                       "Connection: keep-alive\r\n" + append + "Accept-Encoding: deflate\r\n\r\n";
      conn->Write(request);
      auto res = conn->Read(maxSize);
      conn->Close();
      return res;
    }
    String httpsGet(const String &url, const String &append, std::uint64_t maxSize) {
      auto partition = url.find('/');
      auto domain = partition == std::string::npos ? url : url(0, partition);
      auto filename = partition == std::string::npos ? "/" : url(partition, url.size());
      auto addr = ParseIp(domain, "443");
      println(addr.Ip(), addr.Port());
      auto conn = TlsConnection(addr);
      String request = "GET " + filename + " HTTP/1.1\r\n""Host: " + domain +
                       "\r\n""User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:60.0) Gecko/20100101 Firefox/60.0\r\n"
                       "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                       "Connection: keep-alive\r\n" + append + "Accept-Encoding: deflate\r\n\r\n";
      conn.TlsWrite(request);
      auto res = conn.TlsRead(maxSize);
      conn.Close();
      return res;
    }
  }
}