#include <coroutine/go.h>
#include <net/request.h>
#include <iostream>

Coro_Main(argc, argv) {
  auto conn = soranet::http::httpGet("www.baidu.com","80",409600);
  //std::cout << conn << std::endl;
}