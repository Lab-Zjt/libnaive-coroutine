#ifndef UTILITY_VEC_H
#define UTILITY_VEC_H

#include <vector>
#include <list>

namespace srlib {
  template<typename T, typename ...ARGS>
  std::vector<T> Vec(T &&elem, ARGS &&...args) {
    std::vector<T> res;
    res.emplace_back(std::forward<decltype(elem)>(elem));
    char arr[] = {(res.emplace_back(std::forward<decltype(args)>(args)), (char) 0)...};
    return res;
  }
  template<typename T, typename ...ARGS>
  std::list<T> List(T &&elem, ARGS &&...args) {
    std::list<T> res;
    res.emplace_back(std::forward<decltype(elem)>(elem));
    char arr[] = {(res.emplace_back(std::forward<decltype(args)>(args)), (char) 0)...};
    return res;
  }
}

#endif
