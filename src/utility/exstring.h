#ifndef UTILITY_StringING_H
#define UTILITY_StringING_H

#include <string>
#include <ostream>
#include <regex>
#include "strconv.h"

extern "C" {
#include "helper.h"
};

namespace srlib {
  /// String extend std::string.
  /// operator-(str): remove str from tail.
  /// operator()(from,to): equal to substr(from,to-from), if from larger than size(), it will return empty.
  /// split() -> vector<String>.
  /// substitute(old,new) -> replace all old to new in this string.
  /// regex_match(), regex_search(), regex_replace().
  /// map().
  /// trim().
  /// count().
  /// to_lower(), to_upper().
  /// has_prefix(), has_suffix().
  /// to_integer(), to_double().
  /// operator""_s.
  class String {
  private:
    std::string _str;
  public:
    //Iterator
    typedef std::string::iterator iterator;
    typedef std::string::const_iterator const_iterator;
    typedef std::string::const_reverse_iterator const_reverse_iterator;
    typedef std::string::reverse_iterator reverse_iterator;
    //Constructor, Destructor
    ~String() = default;
    String() = default;
    String(const char *str) : _str(str) {}
    String(const char *str, size_t size) : _str(str, size) {}
    String(std::string str) : _str(std::move(str)) {}
    String(const String &rhs) : _str(rhs._str) {}
    String &operator=(const std::string &rhs) {
      _str = rhs;
      return *this;
    }
    String &operator=(const String &rhs) {
      _str = rhs._str;
      return *this;
    }
    String &operator=(std::string &&rhs) {
      _str = std::move(rhs);
      return *this;
    }
    String &operator=(String &&rhs)noexcept {
      _str = std::move(rhs._str);
      return *this;
    }
    //Interaction with std::string
    inline std::string std_string() {return _str;}
    inline std::string &std_string_reference() {return _str;}
    inline const std::string &const_std_string_reference() const {return _str;}
    inline const std::string const_std_string() const {return _str;}
    inline const char *c_str() const {return _str.c_str();}
    inline const char *data() const {return _str.data();}
    inline char &at(size_t pos) {return _str.at(pos);}
    inline const char &at(size_t pos) const {return _str.at(pos);}
    inline char &operator[](int i) {return _str[i];}
    inline const char &operator[](int i) const {return _str[i];}
    inline char &front() {return _str.front();}
    inline const char &front() const {return _str.front();}
    inline char &back() {return _str.back();}
    inline const char &back() const {return _str.back();}
    inline iterator begin() {return _str.begin();}
    inline const_iterator begin() const {return _str.begin();}
    inline const_iterator cbegin() const {return _str.cbegin();}
    inline iterator end() {return _str.end();}
    inline const_iterator end() const {return _str.end();}
    inline const_iterator cend() const {return _str.cend();}
    inline reverse_iterator rbegin() {return _str.rbegin();}
    inline const_reverse_iterator rbegin() const {return _str.rbegin();}
    inline const_reverse_iterator crbegin() const {return _str.crbegin();}
    inline reverse_iterator rend() {return _str.rend();}
    inline const_reverse_iterator rend() const {return _str.rend();};
    inline const_reverse_iterator crend() const {return _str.crend();}
    inline bool empty() const {return _str.empty();}
    inline size_t size() const {return _str.size();}
    inline size_t length() const {return _str.length();}
    inline size_t max_size() const {return _str.max_size();}
    inline String substr(size_t begin, size_t size) const {return String(_str.substr(begin, size));}
    inline void reserve(size_t size) {_str.reserve(size);}
    inline size_t capacity() const {return _str.capacity();}
    inline void shrink_to_fit() {_str.shrink_to_fit();}
    inline void clear() {_str.clear();}
    inline String &insert(size_t index, size_t count, char c) {
      _str.insert(index, count, c);
      return *this;
    }
    inline String &insert(size_t index, const char *sub) {
      _str.insert(index, sub);
      return *this;
    }
    inline String &insert(size_t index, const char *sub, size_t len) {
      _str.insert(index, sub, len);
      return *this;
    }
    inline String &insert(size_t index, const std::string &str) {
      _str.insert(index, str);
      return *this;
    }
    inline String &insert(size_t index, const String &str) {
      _str.insert(index, str._str);
    }
    inline String &append(const char *str) {
      _str.append(str);
      return *this;
    }
    inline String &append(const std::string &str) {
      _str.append(str);
      return *this;
    }
    inline String &append(const String &str) {
      _str.append(str._str);
      return *this;
    }
    inline String &erase(size_t index, size_t size) {
      _str.erase(index, size);
      return *this;
    }
    inline size_t find(const char *sub, size_t from = 0) const {return _str.find(sub, from);}
    inline size_t find(const std::string &sub, size_t from = 0) const {return _str.find(sub, from);}
    inline size_t find(const String &sub, size_t from = 0) const {return _str.find(sub._str, from);}
    inline size_t find(const char c, size_t from = 0) const {return _str.find(c, from);}
    inline size_t rfind(const char *sub, size_t from = std::string::npos) {return _str.rfind(sub, from);}
    inline size_t rfind(const std::string &sub, size_t from = std::string::npos) const {return _str.rfind(sub, from);}
    inline size_t rfind(const String &sub, size_t from = std::string::npos) const {return _str.rfind(sub._str, from);}
    inline size_t rfind(const char c, size_t from = std::string::npos) const {return _str.rfind(c, from);}
    inline size_t find_first_of(const char *sub, size_t from = 0) const {return _str.find_first_of(sub, from);}
    inline size_t find_first_of(const std::string &sub, size_t from = 0) const {return _str.find_first_of(sub, from);}
    inline size_t find_first_of(const String &sub, size_t from = 0) const {return _str.find_first_of(sub._str, from);}
    inline size_t find_first_of(const char c, size_t from = 0) const {return _str.find_first_of(c, from);}
    inline size_t find_first_not_of(const char *sub, size_t from = 0) const {return _str.find_first_not_of(sub, from);}
    inline size_t find_first_not_of(const std::string &sub, size_t from = 0) const {
      return _str.find_first_not_of(sub, from);
    }
    inline size_t find_first_not_of(const String &sub, size_t from = 0) const {
      return _str.find_first_not_of(sub._str, from);
    }
    inline size_t find_first_not_of(const char c, size_t from = 0) const {return _str.find_first_not_of(c, from);}
    inline size_t find_last_of(const char *sub, size_t from = 0) const {return _str.find_last_of(sub, from);}
    inline size_t find_last_of(const std::string &sub, size_t from = 0) const {return _str.find_last_of(sub, from);}
    inline size_t find_last_of(const String &sub, size_t from = 0) const {return _str.find_last_of(sub._str, from);}
    inline size_t find_last_of(const char c, size_t from = 0) const {return _str.find_last_of(c, from);}
    inline size_t find_last_not_of(const char *sub, size_t from = 0) const {return _str.find_last_not_of(sub, from);}
    inline size_t find_last_not_of(const std::string &sub, size_t from = 0) const {
      return _str.find_last_not_of(sub, from);
    }
    inline size_t find_last_not_of(const String &sub, size_t from = 0) const {
      return _str.find_last_not_of(sub._str, from);
    }
    inline size_t find_last_not_of(const char c, size_t from = 0) const {return _str.find_last_not_of(c, from);}
    inline String &replace(size_t index, size_t count, const char *str) {
      _str.replace(index, count, str);
      return *this;
    }
    inline String &replace(size_t index, size_t count, const std::string &str) {
      _str.replace(index, count, str);
      return *this;
    }
    inline String &replace(size_t index, size_t count, const String &str) {
      _str.replace(index, count, str._str);
      return *this;
    }
    inline String &replace(size_t index, size_t count, const char *str, size_t size) {
      _str.replace(index, count, str, size);
      return *this;
    }
    inline void swap(String &rhs) {_str.swap(rhs._str);}
    inline friend String operator+(const String &lhs, const String &rhs) {return String(lhs._str + rhs._str);}
    inline friend String operator+(const String &ex, const std::string &str) {return String(ex._str + str);}
    inline friend String operator+(const std::string &str, const String &ex) {return String(str + ex._str);}
    inline friend String operator+(const char *str, const String &ex) {return String(str) + ex;}
    inline friend String operator+(const String &ex, const char *str) {return ex + String(str);}
    inline friend String operator+(const String &ex, const char rhs) {return String(ex._str + rhs);}
    inline friend String operator+(const char lhs, const String &ex) {return String(lhs + ex._str);}
    inline String &operator+=(const String &rhs) {
      _str += rhs._str;
      return *this;
    }
    inline String &operator+=(const std::string &rhs) {
      _str += rhs;
      return *this;
    }
    inline String &operator+=(const char *rhs) {
      _str += rhs;
      return *this;
    }
    inline String &operator+=(const char rhs) {
      _str += rhs;
      return *this;
    }
    inline friend String operator-(const String &lhs, const String &rhs) {
      return lhs.substr(0, lhs._str.rfind(rhs._str));
    }
    inline friend String operator-(const String &lhs, const std::string &rhs) {
      return lhs.substr(0, lhs._str.rfind(rhs));
    }
    inline friend String operator-(const std::string &lhs, const String &rhs) {return String(lhs) - rhs;}
    inline friend String operator-(const String &lhs, const char *rhs) {return lhs.substr(0, lhs._str.rfind(rhs));}
    inline friend String operator-(const char *lhs, const String &rhs) {return String(lhs) - rhs;}
    inline friend String operator-(const String &lhs, const char rhs) {return lhs.substr(0, lhs._str.rfind(rhs));}
    //FIXME: Bad Performance
    inline String &operator-=(const String &rhs) {
      _str = substr(0, _str.find(rhs._str))._str;
      return *this;
    }
    inline String &operator-=(const std::string &rhs) {
      _str = substr(0, _str.find(rhs))._str;
      return *this;
    }
    inline String &operator-=(const char *rhs) {
      _str = substr(0, _str.find(rhs))._str;
      return *this;
    }
    inline String &operator-=(const char rhs) {
      _str = substr(0, _str.find(rhs))._str;
      return *this;
    }
    inline bool operator<(const String &rhs) const {
      return _str < rhs._str;
    }
    inline bool operator>(const String &rhs) const {
      return rhs < *this;
    }
    inline bool operator<=(const String &rhs) const {
      return !(rhs < *this);
    }
    inline bool operator>=(const String &rhs) const {
      return !(*this < rhs);
    }
    bool operator==(const String &rhs) const {
      return _str == rhs._str;
    }
    bool operator!=(const String &rhs) const {
      return !(rhs == *this);
    }
    friend std::ostream &operator<<(std::ostream &os, const String &String) {
      os << String._str;
      return os;
    }
    //Extend
    bool contains(const char *sub) const {return _str.find(sub) != std::string::npos;}
    bool contains(const char sub) const {return _str.find(sub) != std::string::npos;}
    bool contains(const std::string &sub) const {return _str.find(sub) != std::string::npos;}
    bool contains(const String &sub) const {return _str.find(sub._str) != std::string::npos;}
    inline String operator()(size_t begin, size_t end) const {
      if (begin == std::string::npos)return String();
      if (begin >= end)return String();
      return String(_str.substr(begin, end - begin));
    }
    inline String trim_left(const String &charset, size_t pos = 0) const {
      auto index1 = _str.find_first_not_of(charset._str, pos);
      if (index1 == std::string::npos)
        return String();
      return operator()(index1, size());
    }
    inline String trim_right(const String &charset, size_t pos = std::string::npos) const {
      auto index2 = _str.find_last_not_of(charset._str, pos);
      if (index2 == std::string::npos)
        return String();
      return operator()(0, index2 + 1);
    }
    inline String trim(const String &charset = "\r\t\n \v", size_t pos1 = 0, size_t pos2 = std::string::npos) const {
      auto index1 = _str.find_first_not_of(charset._str, pos1);
      if (index1 == std::string::npos)
        return String();
      auto index2 = _str.find_last_not_of(charset._str, pos2);
      if (index2 == std::string::npos)
        return String();
      return operator()(index1, index2 + 1);
    }
    inline String &remove(size_t from, size_t to) {
      if (from < size()) {
        _str.erase(from, to - from);
      }
      return *this;
    }
    inline String &pop(size_t size) {
      _str.erase(_str.size() - size, size);
      return *this;
    }
    std::vector<String> split(const String &deli) const {
      unsigned long i = 0;
      unsigned long j = 0;
      std::vector<String> res;
      auto strSize = _str.size();
      auto deliSize = deli.size();
      for (;;) {
        j = _str.find(deli._str, i);
        if (i < strSize && i < j)res.push_back(_str.substr(i, j - i));
        if (j == std::string::npos)break;
        i = j + deliSize;
      }
      return res;
    }
    std::vector<String> split(const std::string &deli) const {
      unsigned long i = 0;
      unsigned long j = 0;
      std::vector<String> res;
      auto strSize = _str.size();
      auto deliSize = deli.size();
      for (;;) {
        j = _str.find(deli, i);
        if (i < strSize && i < j)res.push_back(_str.substr(i, j - i));
        if (j == std::string::npos)break;
        i = j + deliSize;
      }
      return res;
    }
    std::vector<String> split(const char *deli) const {
      unsigned long i = 0;
      unsigned long j = 0;
      std::vector<String> res;
      auto strSize = _str.size();
      auto deliSize = strlen(deli);
      for (;;) {
        j = _str.find(deli, i);
        if (i < strSize && i < j)res.push_back(_str.substr(i, j - i));
        if (j == std::string::npos)break;
        i = j + deliSize;
      }
      return res;
    }
    std::vector<String> split(const char deli) const {
      unsigned long i = 0;
      unsigned long j = 0;
      std::vector<String> res;
      auto strSize = _str.size();
      auto deliSize = 1;
      for (;;) {
        j = _str.find(deli, i);
        if (i < strSize && i < j)res.push_back(_str.substr(i, j - i));
        if (j == std::string::npos)break;
        i = j + deliSize;
      }
      return res;
    }
    std::vector<String> filed() const {
      auto size = _str.size();
      auto cstr = _str.c_str();
      std::vector<String> res;
      unsigned pos = 0;
      unsigned start = 0;
      while (pos < size && asciispace[cstr[pos]] != 0)pos++;
      start = pos;
      for (; pos < size;) {
        while (asciispace[cstr[pos]] == 0)pos++;
        res.push_back(_str.substr(start, pos - start));
        while (pos < size && asciispace[cstr[pos]] != 0)pos++;
        start = pos;
      }
      return res;
    }
    size_t count(const String &sub) const {
      size_t res = 0;
      unsigned long pos = 0;
      auto size = sub.size();
      while (true) {
        pos = _str.find(sub._str, pos);
        if (pos == std::string::npos) {
          break;
        }
        pos += size;
        res++;
      }
      return res;
    }
    size_t count(const std::string &sub) const {
      size_t res = 0;
      unsigned long pos = 0;
      auto size = sub.size();
      while (true) {
        pos = _str.find(sub, pos);
        if (pos == std::string::npos) {
          break;
        }
        pos += size;
        res++;
      }
      return res;
    }
    size_t count(const char *sub) const {
      size_t res = 0;
      unsigned long pos = 0;
      auto size = strlen(sub);
      while (true) {
        pos = _str.find(sub, pos);
        if (pos == std::string::npos) {
          break;
        }
        pos += size;
        res++;
      }
      return res;
    }
    size_t count(const char sub) const {
      size_t res = 0;
      unsigned long pos = 0;
      auto size = 1;
      while (true) {
        pos = _str.find(sub, pos);
        if (pos == std::string::npos) {
          break;
        }
        pos += size;
        res++;
      }
      return res;
    }
    String map(std::function<char(char)> &&mapfunc) const {
      auto res(*this);
      for (auto &c:res._str) {
        c = mapfunc(c);
      }
      return res;
    }
    String to_upper() const {
      auto res(*this);
      for (auto &c : res._str) {
        if (islower(c)) {
          c += 'A' - 'a';
        }
      }
      return res;
    }
    String to_lower() const {
      auto res(*this);
      for (auto &c:res._str) {
        if (isupper(c)) {
          c += 'a' - 'A';
        }
      }
      return res;
    }
    std::vector<String> regex_search(const String &str) const {
      std::regex reg(str._str);
      std::smatch match;
      std::vector<String> res;
      std::string buf(_str);
      while (std::regex_search(buf, match, reg)) {
        res.emplace_back(String(std::move(match.str())));
        buf = match.suffix();
      }
      return res;
    }
    std::vector<String> regex_search(const char *str) const {
      std::regex reg(str);
      std::smatch match;
      std::vector<String> res;
      std::string buf(_str);
      while (std::regex_search(buf, match, reg)) {
        res.emplace_back(String(std::move(match.str())));
        buf = match.suffix();
      }
      return res;
    }
    std::vector<String> regex_search(const std::string &str) const {
      std::regex reg(str);
      std::smatch match;
      std::vector<String> res;
      std::string buf(_str);
      while (std::regex_search(buf, match, reg)) {
        res.emplace_back(String(std::move(match.str())));
        buf = match.suffix();
      }
      return res;
    }
    bool regex_match(const char *str) const {
      std::regex reg(str);
      std::smatch match;
      return std::regex_match(_str, match, reg);
    }
    bool regex_match(const std::string &str) const {
      std::regex reg(str);
      std::smatch match;
      return std::regex_match(_str, match, reg);
    }
    bool regex_match(const String &str) const {
      std::regex reg(str.c_str());
      std::smatch match;
      return std::regex_match(_str, match, reg);
    }
    String regex_replace(const char *str, const char *rep) const {
      return String(std::regex_replace(_str, std::regex(str), rep));
    }
    String regex_replace(const std::string &str, std::string &rep) const {
      return String(std::regex_replace(_str, std::regex(str), rep));
    }
    String regex_replace(const String &str, const String &rep) const {
      return String(std::regex_replace(_str, std::regex(str._str), rep._str));
    }
    inline bool has_prefix(const String &pre) const {return _str.find(pre._str) == 0;}
    inline bool has_suffix(const String &suf) const {return _str.find(suf._str) + suf._str.size() == _str.size();}
    String &substitute(const String &old, const String &rep, size_t count = std::string::npos) {
      while (count-- > 0) {
        auto pos = _str.find(old._str);
        if (pos == std::string::npos) {
          break;
        }
        _str.replace(pos, old.size(), rep._str);
      }
      return *this;
    }
    String &substitute(const std::string &old, const std::string &rep, size_t count = std::string::npos) {
      while (count-- > 0) {
        auto pos = _str.find(old);
        if (pos == std::string::npos) {
          break;
        }
        _str.replace(pos, old.size(), rep);
      }
      return *this;
    }
    String &substitute(const char *old, const char *rep, size_t count = std::string::npos) {
      auto size = strlen(rep);
      while (count-- > 0) {
        auto pos = _str.find(old);
        if (pos == std::string::npos) {
          break;
        }
        _str.replace(pos, size, rep);
      }
      return *this;
    }
    // String Convert
    std::int64_t to_integer() const {
      std::int64_t res;
      sscanf(_str.c_str(), "%ld", &res);
      return res;
    };
    std::uint64_t to_unsigned_integer() const {
      std::uint64_t res;
      sscanf(_str.c_str(), "%lu", &res);
      return res;
    }
    float to_float() const {
      float res;
      sscanf(_str.c_str(), "%f", &res);
      return res;
    }
    double to_double() const {
      double res;
      sscanf(_str.c_str(), "%lf", &res);
      return res;
    }
    // Args
    String &ArgInt8(int8_t i) {
      substitute("%hhd", itoa(i), 1);
      return *this;
    }
    String &ArgInt16(int16_t i) {
      substitute("%hd", itoa(i), 1);
      return *this;
    }
    String &ArgInt32(int32_t i) {
      substitute("%d", itoa(i), 1);
      return *this;
    }
    String &ArgInt64(int64_t i) {
      substitute("%ld", itoa(i), 1);
      return *this;
    }
    String &ArgUInt8(uint8_t i) {
      substitute("%hhu", utoa(i), 1);
      return *this;
    }
    String &ArgUInt16(uint16_t i) {
      substitute("%hu", utoa(i), 1);
      return *this;
    }
    String &ArgUInt32(uint32_t i) {
      substitute("%u", utoa(i), 1);
      return *this;
    }
    String &ArgUInt64(uint64_t i) {
      substitute("%lu", utoa(i), 1);
      return *this;
    }
    String &ArgChar(char c) {
      substitute("%c", String(&c, 1), 1);
      return *this;
    }
    String &ArgString(const String &c) {
      substitute("%s", c, 1);
      return *this;
    }
    String &ArgFloat(float f) {
      substitute("%f", ftoa(f), 1);
      return *this;
    }
    String &ArgDouble(double f) {
      substitute("%lf", ftoa(f), 1);
      return *this;
    }
    template<typename T>
    String &Args(T &&t) {
      typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type U;
      if constexpr (std::is_same<U, int8_t>::value) {
        return ArgInt8(t);
      } else if constexpr(std::is_same<U, int16_t>::value) {
        return ArgInt16(t);
      } else if constexpr(std::is_same<U, int32_t>::value) {
        return ArgInt32(t);
      } else if constexpr(std::is_same<U, int64_t>::value) {
        return ArgInt64(t);
      } else if constexpr(std::is_same<U, uint8_t>::value) {
        return ArgUInt8(t);
      } else if constexpr(std::is_same<U, uint16_t>::value) {
        return ArgUInt16(t);
      } else if constexpr(std::is_same<U, uint32_t>::value) {
        return ArgUInt32(t);
      } else if constexpr(std::is_same<U, uint64_t>::value) {
        return ArgUInt64(t);
      } else if constexpr(std::is_same<U, float>::value) {
        return ArgFloat(t);
      } else if constexpr(std::is_same<U, double>::value) {
        return ArgDouble(t);
      } else if constexpr(std::is_same<U, char>::value) {
        return ArgChar(t);
      } else if constexpr(std::is_same<U, char *>::value) {
        return ArgString(t);
      } else if constexpr(std::is_same<U, std::string>::value) {
        return ArgString(t);
      } else if constexpr(std::is_same<U, String>::value) {
        return ArgString(t);
      }
    }
    String &Args() {return *this;}
    template<typename T, typename ...ARGS>
    String &Args(T &&t, ARGS &&...args) {
      Args(t);
      return Args(args...);
    }
  };
};

template<>
struct std::hash<srlib::String> {
  size_t operator()(const srlib::String &str) const {
    return std::hash<std::string>()(str.const_std_string_reference());
  }
};

inline srlib::String operator "" _s(const char *str, size_t size) {
  return srlib::String(str, size);
}
#endif
