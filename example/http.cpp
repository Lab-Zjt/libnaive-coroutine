#include <coroutine/go.h>
#include <utility/exstring.h>
#include <net/request.h>
#include <iostream>
#include <fcntl.h>
#include <utility/io.h>
#include <coroutine/stack_size.h>
#include <net/parse.h>
#include "net/address.h"

using namespace srlib;
using namespace std;
int counter = 0;
std::mutex mut;
Coro_Main(argc, argv) {
  auto conn = net::TlsConnection(net::ParseIp(String(argv[1]), "https"));
  println(net::SendHTTPRequest(conn, net::HTTPRequest{}.AutoFill().Page("/").Header("Host", argv[1])).Serialize());
  return 0;
}