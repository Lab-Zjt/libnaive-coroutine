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
  auto listener = net::Listen(net::Address("0.0.0.0", 10101));
  auto index_page = ReadRegularFile("index.html").substitute("{{.Title}}", title);
  auto upload_page = ReadRegularFile("file.html").substitute("{{.Title}}", title);
  auto success_page = ReadRegularFile("result.html");
  while (true) {
    auto conn = listener->Accept();
    go<32KB>([conn, &upload_page, &index_page, success_page]() mutable {
      auto req = net::RecvHTTPRequest(*conn);
      net::HTTPResponse rep;
      if (req.method == "GET") {
        if (req.page == "/") {
          rep.AutoFill()
             .Header("Content-Length", itoa(index_page.size()))
             .Header("Content-Type", "text/html")
             .Content(index_page);
        } else if (req.page == "/upload") {
          rep.AutoFill()
             .Header("Content-Length", itoa(upload_page.size()))
             .Header("Content-Type", "text/html")
             .Content(upload_page);
        } else {
          rep.StatusCode("404")
             .ReasonPhrase("Not Found")
             .Content("404 Not Found")
             .Header("Content-Length", itoa(13))
             .Header("Content-Type", "text/html");
        }
      } else if (req.method == "POST") {
        auto &content = req.content;
        auto &content_type = req.header["Content-Type"];
        auto boundary = content_type(content_type.find("boundary=") + 9, content_type.size());
        auto body = content(content.find(boundary), content.rfind(boundary));
        auto filebody = body(body.find("\r\n\r\n"), body.size());
        auto filename_start = body.find("filename=\"");
        auto filename = body(filename_start + 10, body.find("\"", filename_start + 10));
        OpenFile(filename).Write(filebody);
        success_page.substitute("{{.FileName}}", filename).substitute("{{.Size}}", std::to_string(filebody.size()));
        rep.AutoFill()
           .Header("Content-Length", itoa(success_page.size()))
           .Content(success_page)
           .Header("Content-Type", "text/html");
      }
      conn->Write(rep.Serialize());
    });
  }
}