#ifndef SORANET_STRCONV_H
#define SORANET_STRCONV_H

#include <cstdio>

namespace srlib {
  inline std::string itoa(std::int64_t i) {
    char buf[32];
    sprintf(buf, "%ld", i);
    return std::string(buf);
  }
  inline std::string utoa(std::uint64_t u) {
    char buf[32];
    sprintf(buf, "%lu", u);
    return std::string(buf);
  }
  inline std::string ftoa(double d) {
    char buf[128];
    sprintf(buf, "%lf", d);
    return std::string(buf);
  }
  inline std::string ftoa(double d, int suffix) {
    char buf[128];
    sprintf(buf, "%.*lf", suffix, d);
    return std::string(buf);
  }
  inline std::string ftoa(double d, int prefix, int suffix) {
    char buf[128];
    sprintf(buf, "%*.*lf", prefix, suffix, d);
    return std::string(buf);
  }
}

#endif
