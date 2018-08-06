#ifndef SORANET_REQUEST_H
#define SORANET_REQUEST_H

#include <string>

namespace soranet {
  namespace http {
    std::string httpGet(const std::string &url, const std::string &append = {}, std::uint64_t maxSize = 40960);
    std::string httpsGet(const std::string &url, const std::string &append = {}, std::uint64_t maxSize = 40960);
    std::string Get(const std::string &url);
    std::string httpHead(const std::string &url);
    std::string httpsHead(const std::string &url);
    std::string Head(const std::string &url);
  }
}
#endif
