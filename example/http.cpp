#include <coroutine/go.h>
#include <utility/exstring.h>
#include <net/request.h>
#include <iostream>
#include <fcntl.h>

using namespace srlib;
using namespace std;
int counter = 0;
std::mutex mut;
Coro_Main(argc, argv) {
  String str;
  bool flag = false;
  go([&str, &flag]() {
    str = net::httpsGet("www.sorakasugano.com", {}, 409600);
    flag = true;
  });
  auto resp = net::httpsGet("www.bilibili.com", {}, 409600);
  cout << resp << endl;
  while (!flag) {}
  cout << str << endl;
  return 0;
}