#include <coroutine/go.h>
#include <net/request.h>
#include <iostream>
#include <fcntl.h>

using namespace soranet;
using namespace std;
int counter = 0;
std::mutex mut;
Coro_Main(argc, argv) {
  std::string str;
  bool flag = false;
  go([&str, &flag]() {
    str = http::httpsGet("www.sorakasugano.com", {}, 409600);
    flag = true;
  });
  auto resp = http::httpsGet("www.bilibili.com", {}, 409600);
  cout << resp << endl;
  while (!flag) {}
  cout << str << endl;
  return 0;
}