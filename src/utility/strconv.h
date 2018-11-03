#ifndef SORANET_STRCONV_H
#define SORANET_STRCONV_H

#include <string>

namespace srlib {
  /// Low performance string convert. Use sprintf() (for floating point because of it allow you custom suffix's length) and to_string() (for integer).
  template<typename T, typename U =typename std::enable_if<std::is_integral<T>::value>::type>
  inline std::string itoa(T i) {
    return std::to_string(i);
  }
  template<typename T, typename U =typename std::enable_if<std::is_floating_point<T>::value>::type>
  inline std::string ftoa(T f, int suffix = 6) {
    char buf[128];
    sprintf(buf, "%.*lf", suffix, f);
    return std::string(buf);
  }
  template<typename T, typename U=typename std::enable_if<std::is_pointer<T>::value>::type>
  inline std::string ptoa(T p) {
    char buf[32];
    sprintf(buf, "%p", p);
    return std::string(buf);
  }
}

#endif
