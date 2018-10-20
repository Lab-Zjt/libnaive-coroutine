#include <utility/io.h>
#include "utility/exstring.h"
#include "connector.h"
#include "connection.h"
#include "address.h"
#include "tls_connection.h"
#include "parse.h"
#include "request.h"

namespace srlib {
  namespace net {
    String HTTPRequest::Serialize() {
      auto res = method + ' ' + page + " HTTP/" + version + "\r\n";
      for (auto &field : header) {
        res += field.first + ": " + field.second + "\r\n";
      }
      res += "\r\n";
      res += content;
      return res;
    }
    HTTPRequest HTTPRequest::Unserialize(const srlib::String &req) {
      HTTPRequest res;
      if (req.empty())
        return res;
      auto header_end = req.find("\r\n\r\n");
      auto header = req(0, header_end).split("\r\n");
      res.content = req(header_end + 4, req.size());
      res.method = header[0](0, header[0].find(' '));
      res.page = header[0](header[0].find('/'), header[0].rfind("HTTP/") - 1);
      res.version = header[0](header[0].rfind("HTTP/") + 5, header[0].size());
      for (int i = 1; i < header.size(); ++i) {
        auto sp = header[i].find(": ");
        res.header[header[i](0, sp)] = header[i](sp + 2, header[i].size());
      }
      return res;
    }
    String HTTPResponse::Serialize() {
      auto res = "HTTP/" + version + ' ' + status_code + ' ' + reason_phrase + "\r\n";
      for (auto &field : header) {
        res += field.first + ": " + field.second + "\r\n";
      }
      res += "\r\n";
      res += content;
      return res;
    }
    HTTPResponse HTTPResponse::Unserialize(const srlib::String &req) {
      HTTPResponse res;
      auto header_end = req.find("\r\n\r\n");
      auto header = req(0, header_end).split("\r\n");
      res.content = req(header_end + 4, req.size());
      auto status_start = header[0].find(' ') + 1;
      res.version = header[0](header[0].find("HTTP/") + 5, status_start - 1);
      auto reason_start = header[0].find(' ', status_start) + 1;
      res.status_code = header[0](status_start, reason_start - 1);
      res.reason_phrase = header[0](reason_start, header[0].size());
      for (int i = 1; i < header.size(); ++i) {
        auto sp = header[i].find(": ");
        res.header[header[i](0, sp)] = header[i](sp + 2, header[i].size());
      }
      return res;
    }
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