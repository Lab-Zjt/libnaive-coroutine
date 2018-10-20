#ifndef UTILITY_REQUEST_H
#define UTILITY_REQUEST_H

#include "utility/exstring.h"

namespace srlib {
  class String;
  namespace net {
    struct HTTPRequest {
      String method = "GET";
      String page = "/";
      String version = "1.1";
      std::map<String, String> header;
      String content = "";
      String Serialize() {
        auto res = method + ' ' + page + " HTTP/" + version + "\r\n";
        for (auto &field : header) {
          res += field.first + ": " + field.second + "\r\n";
        }
        res += "\r\n";
        res += content;
        return res;
      }
      static HTTPRequest Unserialize(const String &req) {
        HTTPRequest res;
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
    };
    
    struct HTTPResponse {
      String version = "1.1";
      String status_code = "200";
      String reason_phrase = "OK";
      std::map<String, String> header;
      String content;
      String Serialize() {
        auto res = "HTTP/" + version + ' ' + status_code + ' ' + reason_phrase + "\r\n";
        for (auto &field : header) {
          res += field.first + ": " + field.second;
        }
        res += "\r\n";
        res += content;
        return res;
      }
      static HTTPResponse Unserialize(const String &req) {
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
    };
    
    String httpGet(const String &url, const String &append = {}, std::uint64_t maxSize = 1024 * 1024);
    String httpsGet(const String &url, const String &append = {}, std::uint64_t maxSize = 1024 * 1024);
  }
}

#endif
