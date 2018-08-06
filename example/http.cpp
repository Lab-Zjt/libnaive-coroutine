#include <coroutine/go.h>
#include <net/request.h>
#include <iostream>
#include <fcntl.h>

Coro_Main(argc, argv) {
  auto resp = soranet::http::httpsGet("www.bilibili.com", {}, 409600);
  std::cout << resp << std::endl;
  return 0;
}