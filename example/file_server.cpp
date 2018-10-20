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
  /*auto conn = net::TcpConnection("67.209.179.32", "25252");
  conn.Write("GET / HTTP/1.1\r\n"
             "Host: 67.209.179.32:25252\r\n"
             "User-Agent: curl\r\n\r\n");
  auto str = conn.Read(1024);
  println(str);
  auto req = net::HTTPResponse::Unserialize(str);*/
  auto listener = net::Listen(net::Address("0.0.0.0", 10101));
  auto index_page = ReadRegularFile("index.html").substitute("{{.Title}}", title);
  auto upload_page = ReadRegularFile("file.html").substitute("{{.Title}}", title);
  auto success_page = ReadRegularFile("result.html");
  while (true) {
    auto conn = listener->Accept();
    go<32KB>([conn, &upload_page, &index_page, success_page]() mutable {
      auto str = conn->Read(4096);
      auto req = net::HTTPRequest::Unserialize(str);
      net::HTTPResponse rep;
      if (req.method == "GET") {
        if (req.page == "/") {
          rep.header["Content-Length"] = itoa(index_page.size());
          rep.header["Content-Type"] = "text/html"_s;
          rep.content = index_page;
        } else if (req.page == "/upload") {
          rep.header["Content-Length"] = itoa(upload_page.size());
          rep.header["Content-Type"] = "text/html"_s;
          rep.content = upload_page;
        } else {
          rep.status_code = "404"_s;
          rep.reason_phrase = "Not Found"_s;
          rep.content = "404 Not Found"_s;
          rep.header["Content-Length"] = itoa("404 Not Found"_s.size());
          rep.header["Content-Type"] = "text/html"_s;
        }
      } else if (req.method == "POST") {
        auto size = atoi(req.header["Content-Length"].c_str());
        auto content = conn->Read(size);
        auto &content_type = req.header["Content-Type"];
        auto boundary = content_type(content_type.find("boundary=") + 9, content_type.size());
        auto body = content(content.find(boundary), content.rfind(boundary));
        auto filebody = body(body.find("\r\n\r\n"), body.size());
        auto filename_start = body.find("filename=\"");
        auto filename = body(filename_start + 10, body.find("\"", filename_start + 10));
        OpenFile(filename).Write(filebody);
      }
      conn->Write(rep.Serialize());
    });
  }
}