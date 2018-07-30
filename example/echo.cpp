#include <coroutine/go.h>
#include <net/listener.h>
#include <net/connection.h>
#include <net/address.h>

Coro_Main(argc, argv) {
  auto listener = soranet::Listen("0.0.0.0", 1212);
  while (true) {
    std::shared_ptr<soranet::Connection> conn(listener.accpet());
    go([conn]() {
      auto str = conn->read(1024);
      conn->write(str);
      conn->close();
    });
  }
}