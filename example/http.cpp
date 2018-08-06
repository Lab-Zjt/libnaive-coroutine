#include <coroutine/go.h>
#include <net/request.h>

Coro_Main(argc, argv) {
  soranet::http::httpGet("www.baidu.com", "80", 204800);
}