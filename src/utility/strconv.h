#ifndef SORANET_STRCONV_H
#define SORANET_STRCONV_H

#include <string>

namespace srlib {
  /// Low performance string convert. Use sprintf().
  /// func will use static buffer, func_s() will use auto buffer.
  static char num[11] = "0123456789";
  inline std::string itoa(std::int64_t i) {
    char buf[20];
    if (i == 0) {
      return std::string("0");
    }
    bool neg = i < 0;
    i = i > 0 ? i : -i;
    size_t off = 20;
    while (i != 0) {
      buf[--off] = num[i % 10];
      i /= 10;
    }
    if (neg)buf[--off] = '-';
    return std::string(buf + off, 20 - off);
  }
  inline std::string utoa(std::uint64_t i) {
    char buf[20];
    if (i == 0) {
      return std::string("0");
    }
    size_t off = 20;
    while (i != 0) {
      buf[--off] = num[i % 10];
      i /= 10;
    }
    return std::string(buf + off, 20 - off);
  }
  inline std::string ftoa(double d, int suffix = 6) {
    static char buf[128];
    sprintf(buf, "%.*lf", suffix, d);
    return std::string(buf);
  }
  inline std::string ftoa_s(double d, int suffix = 6) {
    char buf[128];
    sprintf(buf, "%.*lf", suffix, d);
    return std::string(buf);
  }
  
}

#endif
