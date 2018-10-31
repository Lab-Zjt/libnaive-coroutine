#include <coroutine/go.h>
#include <utility/exstring.h>
#include <net/request.h>
#include <iostream>
#include <fcntl.h>
#include <utility/io.h>
#include <coroutine/stack_size.h>

using namespace srlib;
using namespace std;
int counter = 0;
std::mutex mut;
Coro_Main(argc, argv) {
  vector<vector<String>> res;
  bool flag = false;
  go<16KB>([&res, &flag]() {
    auto str = net::httpsGet("www.bilibili.com");
    flag = true;
  });
  auto resp = net::httpsGet("www.baidu.com");
  cout << resp << endl;
  while (!flag) {}
  for (auto &line:res) {
    for (auto &str:line) {
      println(str);
    }
  }
  return 0;
}