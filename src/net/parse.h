#ifndef UTILITY_URL_PARSER_H
#define UTILITY_URL_PARSER_H

#include "utility/exstring.h"
namespace srlib {
  class String;
  namespace net {
    class Address;
    Address ParseIp(const String &url, const String &service = "1");
  }
}

#endif
