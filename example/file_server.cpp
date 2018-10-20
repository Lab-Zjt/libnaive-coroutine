#include <utility/io.h>
#include <net/request.h>
#include "net/listener.h"
#include "net/address.h"
#include "coroutine/go.h"
#include "coroutine/stack_size.h"
#include "utility/exstring.h"

using namespace srlib;

const char *title = "Upload";
Coro_Main(argc, argv) {
  auto conn = net::TcpConnection("67.209.179.32", "25252");
  conn.Write("GET / HTTP/1.1\r\n"
             "Host: 67.209.179.32:25252\r\n"
             "User-Agent: curl\r\n\r\n");
  auto str = conn.Read(1024);
  println(str);
  auto req = net::HTTPResponse::Unserialize(str);
  return 0;
  auto listener = net::Listen(net::Address("0.0.0.0", 10101));
  auto upload_page = ReadRegularFile("file.html").substitute("{{.Title}}", title);
  while (true) {
    auto conn = listener->Accept();
    go<16KB>([conn, upload_page]() mutable {
      auto req = conn->Read(4096);
      println(req);
      conn->Write("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n"
                  "Content-Length: " + itoa(upload_page.size()) + "\r\n\r\n" + upload_page);
      auto content = conn->Read(10000);
      println(content);
    });
  }
}