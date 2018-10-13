#include <coroutine/go.h>
#include <net/listener.h>
#include <net/connection.h>
#include <net/address.h>

using namespace srlib;
Coro_Main(argc, argv) {
  auto listener = net::Listen(net::Address("0.0.0.0", 1212));
  while (true) {
    auto conn = listener->Accept();
    go([conn]() {
      auto str = conn->Read(1024);
      conn->Write(str);
      conn->Close();
    });
  }
}