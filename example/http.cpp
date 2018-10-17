#include <coroutine/go.h>
#include <utility/exstring.h>
#include <net/request.h>
#include <iostream>
#include <fcntl.h>
#include <utility/io.h>

using namespace srlib;
using namespace std;
int counter = 0;
std::mutex mut;
Coro_Main(argc, argv) {
  vector<vector<String>> res;
  bool flag = false;
  go([&res, &flag]() {
    auto str = net::httpsGet("www.sorakasugano.com", {}, 409600);
    auto content = str.split("\r\n");
    for (auto &line:content) {
      res.emplace_back(line.split('\n'));
    }
    flag = true;
  });
  auto resp = net::httpsGet("www.bilibili.com", {}, 409600);
  cout << resp << endl;
  while (!flag) {}
  for (auto &line:res) {
    for (auto &str:line) {
      println(str);
    }
  }
  return 0;
}