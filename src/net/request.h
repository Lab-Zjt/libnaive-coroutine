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
      String Serialize();
      static HTTPRequest Unserialize(const String &req);
    };
    
    struct HTTPResponse {
      String version = "1.1";
      String status_code = "200";
      String reason_phrase = "OK";
      std::map<String, String> header;
      String content;
      String Serialize();
      static HTTPResponse Unserialize(const String &req);
    };
    
    String httpGet(const String &url, const String &append = {}, std::uint64_t maxSize = 1024 * 1024);
    String httpsGet(const String &url, const String &append = {}, std::uint64_t maxSize = 1024 * 1024);
  }
}

#endif
