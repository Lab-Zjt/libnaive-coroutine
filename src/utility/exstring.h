#ifndef UTILITY_EXSTRING_H
#define UTILITY_EXSTRING_H

#include <string>
#include <ostream>
#include <regex>

extern "C" {
#include "helper.h"
};

namespace srlib {
  class exstr {
  private:
    std::string _str;
  public:
    //Iterator
    typedef std::string::iterator iterator;
    typedef std::string::const_iterator const_iterator;
    typedef std::string::const_reverse_iterator const_reverse_iterator;
    typedef std::string::reverse_iterator reverse_iterator;
    //Constructor, Destructor
    ~exstr() = default;
    exstr() = default;
    exstr(const char *str) : _str(str) {}
    exstr(const char *str, size_t size) : _str(str, size) {}
    exstr(std::string str) : _str(std::move(str)) {}
    exstr(const exstr &rhs) : _str(rhs._str) {}
    exstr &operator=(const std::string &rhs) {
      _str = rhs;
      return *this;
    }
    exstr &operator=(const exstr &rhs) {
      _str = rhs._str;
      return *this;
    }
    exstr &operator=(std::string &&rhs) {
      _str = std::move(rhs);
      return *this;
    }
    exstr &operator=(exstr &&rhs)noexcept {
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
    inline exstr substr(size_t begin, size_t size) const {return exstr(_str.substr(begin, size));}
    inline void reserve(size_t size) {_str.reserve(size);}
    inline size_t capacity() const {return _str.capacity();}
    inline void shrink_to_fit() {_str.shrink_to_fit();}
    inline void clear() {_str.clear();}
    inline exstr &insert(size_t index, size_t count, char c) {
      _str.insert(index, count, c);
      return *this;
    }
    inline exstr &insert(size_t index, const char *sub) {
      _str.insert(index, sub);
      return *this;
    }
    inline exstr &insert(size_t index, const char *sub, size_t len) {
      _str.insert(index, sub, len);
      return *this;
    }
    inline exstr &insert(size_t index, const std::string &str) {
      _str.insert(index, str);
      return *this;
    }
    inline exstr &insert(size_t index, const exstr &str) {
      _str.insert(index, str._str);
    }
    inline exstr &append(const char *str) {
      _str.append(str);
      return *this;
    }
    inline exstr &append(const std::string &str) {
      _str.append(str);
      return *this;
    }
    inline exstr &append(const exstr &str) {
      _str.append(str._str);
      return *this;
    }
    inline exstr &erase(size_t index, size_t size) {
      _str.erase(index, size);
      return *this;
    }
    inline size_t find(const char *sub, size_t from = 0) const {return _str.find(sub, from);}
    inline size_t find(const std::string &sub, size_t from = 0) const {return _str.find(sub, from);}
    inline size_t find(const exstr &sub, size_t from = 0) const {return _str.find(sub._str, from);}
    inline size_t find(const char c, size_t from = 0) const {return _str.find(c, from);}
    inline size_t rfind(const char *sub, size_t from = std::string::npos) {return _str.rfind(sub, from);}
    inline size_t rfind(const std::string &sub, size_t from = std::string::npos) const {return _str.rfind(sub, from);}
    inline size_t rfind(const exstr &sub, size_t from = std::string::npos) const {return _str.rfind(sub._str, from);}
    inline size_t rfind(const char c, size_t from = std::string::npos) const {return _str.rfind(c, from);}
    inline size_t find_first_of(const char *sub, size_t from = 0) const {return _str.find_first_of(sub, from);}
    inline size_t find_first_of(const std::string &sub, size_t from = 0) const {return _str.find_first_of(sub, from);}
    inline size_t find_first_of(const exstr &sub, size_t from = 0) const {return _str.find_first_of(sub._str, from);}
    inline size_t find_first_of(const char c, size_t from = 0) const {return _str.find_first_of(c, from);}
    inline size_t find_first_not_of(const char *sub, size_t from = 0) const {return _str.find_first_not_of(sub, from);}
    inline size_t find_first_not_of(const std::string &sub, size_t from = 0) const {
      return _str.find_first_not_of(sub, from);
    }
    inline size_t find_first_not_of(const exstr &sub, size_t from = 0) const {
      return _str.find_first_not_of(sub._str, from);
    }
    inline size_t find_first_not_of(const char c, size_t from = 0) const {return _str.find_first_not_of(c, from);}
    inline size_t find_last_of(const char *sub, size_t from = 0) const {return _str.find_last_of(sub, from);}
    inline size_t find_last_of(const std::string &sub, size_t from = 0) const {return _str.find_last_of(sub, from);}
    inline size_t find_last_of(const exstr &sub, size_t from = 0) const {return _str.find_last_of(sub._str, from);}
    inline size_t find_last_of(const char c, size_t from = 0) const {return _str.find_last_of(c, from);}
    inline size_t find_last_not_of(const char *sub, size_t from = 0) const {return _str.find_last_not_of(sub, from);}
    inline size_t find_last_not_of(const std::string &sub, size_t from = 0) const {
      return _str.find_last_not_of(sub, from);
    }
    inline size_t find_last_not_of(const exstr &sub, size_t from = 0) const {
      return _str.find_last_not_of(sub._str, from);
    }
    inline size_t find_last_not_of(const char c, size_t from = 0) const {return _str.find_last_not_of(c, from);}
    inline exstr &replace(size_t index, size_t count, const char *str) {
      _str.replace(index, count, str);
      return *this;
    }
    inline exstr &replace(size_t index, size_t count, const std::string &str) {
      _str.replace(index, count, str);
      return *this;
    }
    inline exstr &replace(size_t index, size_t count, const exstr &str) {
      _str.replace(index, count, str._str);
      return *this;
    }
    inline exstr &replace(size_t index, size_t count, const char *str, size_t size) {
      _str.replace(index, count, str, size);
      return *this;
    }
    inline void swap(exstr &rhs) {_str.swap(rhs._str);}
    inline friend exstr operator+(const exstr &lhs, const exstr &rhs) {return exstr(lhs._str + rhs._str);}
    inline friend exstr operator+(const exstr &ex, const std::string &str) {return exstr(ex._str + str);}
    inline friend exstr operator+(const std::string &str, const exstr &ex) {return exstr(str + ex._str);}
    inline friend exstr operator+(const char *str, const exstr &ex) {return exstr(str) + ex;}
    inline friend exstr operator+(const exstr &ex, const char *str) {return ex + exstr(str);}
    inline friend exstr operator+(const exstr &ex, const char rhs) {return exstr(ex._str + rhs);}
    inline friend exstr operator+(const char lhs, const exstr &ex) {return exstr(lhs + ex._str);}
    inline exstr &operator+=(const exstr &rhs) {
      _str += rhs._str;
      return *this;
    }
    inline exstr &operator+=(const std::string &rhs) {
      _str += rhs;
      return *this;
    }
    inline exstr &operator+=(const char *rhs) {
      _str += rhs;
      return *this;
    }
    inline exstr &operator+=(const char rhs) {
      _str += rhs;
      return *this;
    }
    inline friend exstr operator-(const exstr &lhs, const exstr &rhs) {return lhs.substr(0, lhs._str.rfind(rhs._str));}
    inline friend exstr operator-(const exstr &lhs, const std::string &rhs) {return lhs.substr(0, lhs._str.rfind(rhs));}
    inline friend exstr operator-(const std::string &lhs, const exstr &rhs) {return exstr(lhs) - rhs;}
    inline friend exstr operator-(const exstr &lhs, const char *rhs) {return lhs.substr(0, lhs._str.rfind(rhs));}
    inline friend exstr operator-(const char *lhs, const exstr &rhs) {return exstr(lhs) - rhs;}
    inline friend exstr operator-(const exstr &lhs, const char rhs) {return lhs.substr(0, lhs._str.rfind(rhs));}
    //FIXME: Bad Performance
    inline exstr &operator-=(const exstr &rhs) {
      _str = substr(0, _str.find(rhs._str))._str;
      return *this;
    }
    inline exstr &operator-=(const std::string &rhs) {
      _str = substr(0, _str.find(rhs))._str;
      return *this;
    }
    inline exstr &operator-=(const char *rhs) {
      _str = substr(0, _str.find(rhs))._str;
      return *this;
    }
    inline exstr &operator-=(const char rhs) {
      _str = substr(0, _str.find(rhs))._str;
      return *this;
    }
    inline bool operator<(const exstr &rhs) const {
      return _str < rhs._str;
    }
    inline bool operator>(const exstr &rhs) const {
      return rhs < *this;
    }
    inline bool operator<=(const exstr &rhs) const {
      return !(rhs < *this);
    }
    inline bool operator>=(const exstr &rhs) const {
      return !(*this < rhs);
    }
    bool operator==(const exstr &rhs) const {
      return _str == rhs._str;
    }
    bool operator!=(const exstr &rhs) const {
      return !(rhs == *this);
    }
    friend std::ostream &operator<<(std::ostream &os, const exstr &exstr) {
      os << exstr._str;
      return os;
    }
    //Extend
    bool contains(const char *sub) const {return _str.find(sub) != std::string::npos;}
    bool contains(const char sub) const {return _str.find(sub) != std::string::npos;}
    bool contains(const std::string &sub) const {return _str.find(sub) != std::string::npos;}
    bool contains(const exstr &sub) const {return _str.find(sub._str) != std::string::npos;}
    inline exstr operator()(size_t begin, size_t end) const {
      if (begin == std::string::npos)return exstr();
      if (begin >= end)return exstr();
      return exstr(_str.substr(begin, end - begin));
    }
    inline exstr &remove(size_t from, size_t to) {
      if (from < size()) {
        _str.erase(from, to - from);
      }
      return *this;
    }
    inline exstr &pop(size_t size) {
      _str.erase(_str.size() - size, size);
      return *this;
    }
    std::vector<exstr> split(const exstr &deli) const {
      unsigned long i = 0;
      unsigned long j = 0;
      std::vector<exstr> res;
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
    std::vector<exstr> split(const std::string &deli) const {
      unsigned long i = 0;
      unsigned long j = 0;
      std::vector<exstr> res;
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
    std::vector<exstr> split(const char *deli) const {
      unsigned long i = 0;
      unsigned long j = 0;
      std::vector<exstr> res;
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
    std::vector<exstr> split(const char deli) const {
      unsigned long i = 0;
      unsigned long j = 0;
      std::vector<exstr> res;
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
    std::vector<exstr> filed() const {
      auto size = _str.size();
      auto cstr = _str.c_str();
      std::vector<exstr> res;
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
    size_t count(const exstr &sub) const {
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
    exstr map(std::function<char(char)> &&mapfunc) const {
      auto res(*this);
      for (auto &c:res._str) {
        c = mapfunc(c);
      }
      return res;
    }
    exstr to_upper() const {
      auto res(*this);
      for (auto &c : res._str) {
        if (islower(c)) {
          c += 'A' - 'a';
        }
      }
      return res;
    }
    exstr to_lower() const {
      auto res(*this);
      for (auto &c:res._str) {
        if (isupper(c)) {
          c += 'a' - 'A';
        }
      }
      return res;
    }
    std::vector<exstr> regex_search(const exstr &str) {
      std::regex reg(str._str);
      std::smatch match;
      std::vector<exstr> res;
      std::string buf(_str);
      while (std::regex_search(buf, match, reg)) {
        res.emplace_back(exstr(std::move(match.str())));
        buf = match.suffix();
      }
      return res;
    }
    std::vector<exstr> regex_search(const char *str) {
      std::regex reg(str);
      std::smatch match;
      std::vector<exstr> res;
      std::string buf(_str);
      while (std::regex_search(buf, match, reg)) {
        res.emplace_back(exstr(std::move(match.str())));
        buf = match.suffix();
      }
      return res;
    }
    std::vector<exstr> regex_search(const std::string &str) {
      std::regex reg(str);
      std::smatch match;
      std::vector<exstr> res;
      std::string buf(_str);
      while (std::regex_search(buf, match, reg)) {
        res.emplace_back(exstr(std::move(match.str())));
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
    bool regex_match(const exstr &str) const {
      std::regex reg(str.c_str());
      std::smatch match;
      return std::regex_match(_str, match, reg);
    }
    exstr regex_replace(const char *str, const char *rep) const {
      return exstr(std::regex_replace(_str, std::regex(str), rep));
    }
    exstr regex_replace(const std::string &str, std::string &rep) const {
      return exstr(std::regex_replace(_str, std::regex(str), rep));
    }
    exstr regex_replace(const exstr &str, const exstr &rep) const {
      return exstr(std::regex_replace(_str, std::regex(str._str), rep._str));
    }
    inline bool has_prefix(const exstr &pre) const {return _str.find(pre._str) == 0;}
    inline bool has_suffix(const exstr &suf) const {return _str.find(suf._str) + suf._str.size() == _str.size();}
    exstr &substitute(const exstr &old, const exstr &rep, size_t count = std::string::npos) {
      while (count-- > 0) {
        auto pos = _str.find(old._str);
        if (pos == std::string::npos) {
          break;
        }
        _str.replace(pos, old.size(), rep._str);
      }
      return *this;
    }
    exstr &substitute(const std::string &old, const std::string &rep, size_t count = std::string::npos) {
      while (count-- > 0) {
        auto pos = _str.find(old);
        if (pos == std::string::npos) {
          break;
        }
        _str.replace(pos, old.size(), rep);
      }
      return *this;
    }
    exstr &substitute(const char *old, const char *rep, size_t count = std::string::npos) {
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
  };
}
template<>
struct std::hash<srlib::exstr> {
  size_t operator()(const srlib::exstr &str) const {
    return std::hash<std::string>()(str.const_std_string_reference());
  }
};

srlib::exstr operator "" _s(const char *str, size_t size) {
  return srlib::exstr(str, size);
}
#endif
