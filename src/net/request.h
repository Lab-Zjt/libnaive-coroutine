#ifndef UTILITY_REQUEST_H
#define UTILITY_REQUEST_H

namespace srlib {
  class String;
  namespace net {
    String httpGet(const String &url, const String &append = {}, std::uint64_t maxSize = 1024 * 1024);
    String httpsGet(const String &url, const String &append = {}, std::uint64_t maxSize = 1024 * 1024);
  }
}

#endif
