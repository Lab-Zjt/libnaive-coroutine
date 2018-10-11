#include <coroutine/go.h>
#include <net/request.h>
#include <utility/exstring.h>
#include <utility/strconv.h>
#include <iostream>
#include <fcntl.h>

using namespace soranet;
using namespace srlib;
using namespace std;
int counter = 0;
std::mutex mut;
Coro_Main(argc, argv) {
  auto url = "www.sorakasugano.com"_s;
  exstr resp = http::httpsGet(url.const_std_string_reference(), {}, 409600);
  auto list = resp.regex_search(R"(href="[^"]+")");
  for (auto &l : list) {
    l = l.regex_replace(R"(href="([^"]+)\")", "$1");
    go([l, url]()mutable {
      url += l;
      if (!url.regex_match(R"(www.sorakasugano.com/.*)"))return;
      auto res = http::httpsGet(url.const_std_string_reference());
      mut.lock();
      Pipe f(::open(itoa(counter++).c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666));
      mut.unlock();
      f.write(res);
      f.close();
    });
  }
  return 0;
}