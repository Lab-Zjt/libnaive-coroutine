#include <net/request.h>
#include <net/connection.h>
#include <net/tlsconn.h>

namespace soranet {
  namespace http {
    std::string httpGet(const std::string &url, const std::string &append, std::uint64_t maxSize) {
      auto partition = url.find('/');
      auto domain = partition == std::string::npos ? url : url.substr(0, partition);
      auto filename = partition == std::string::npos ? "/" : url.substr(partition);
      auto conn = httpDial(domain);
      std::string request = "GET " + filename + " HTTP/1.1\r\n""Host: " + domain +
                            "\r\n""User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:60.0) Gecko/20100101 Firefox/60.0\r\n"
                            "Accept: */*\r\n"
                            "Connection: keep-alive\r\n" + append + "Accept-Encoding: deflate\r\n\r\n";
      conn->write(request);
      auto res = conn->read(maxSize);
      conn->close();
      return res;
    }
    std::string httpsGet(const std::string &url, const std::string &append, std::uint64_t maxSize) {
      auto partition = url.find('/');
      auto domain = partition == std::string::npos ? url : url.substr(0, partition);
      auto filename = partition == std::string::npos ? "/" : url.substr(partition);
      auto conn = httpsDial(domain);
      printf("url = %s, conn = %p\n", url.c_str(), conn.get());
      std::string request = "GET " + filename + " HTTP/1.1\r\n""Host: " + domain +
                            "\r\n""User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:60.0) Gecko/20100101 Firefox/60.0\r\n"
                            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                            "Connection: keep-alive\r\n" + append + "Accept-Encoding: deflate\r\n\r\n";
      conn->tlsWrite(request);
      auto res = conn->tlsRead(maxSize);
      conn->close();
      return res;
    }
  }
}