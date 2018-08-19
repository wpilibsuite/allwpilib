/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cstdio>

#include "wpi/EventLoopRunner.h"
#include "wpi/HttpServerConnection.h"
#include "wpi/UrlParser.h"
#include "wpi/raw_ostream.h"
#include "wpi/uv/Loop.h"
#include "wpi/uv/Tcp.h"

namespace uv = wpi::uv;

class MyHttpServerConnection : public wpi::HttpServerConnection {
 public:
  explicit MyHttpServerConnection(std::shared_ptr<uv::Stream> stream)
      : HttpServerConnection(stream) {}

 protected:
  void ProcessRequest() override;
};

void MyHttpServerConnection::ProcessRequest() {
  wpi::errs() << "HTTP request: '" << m_request.GetUrl() << "'\n";
  wpi::UrlParser url{m_request.GetUrl(),
                     m_request.GetMethod() == wpi::HTTP_CONNECT};
  if (!url.IsValid()) {
    // failed to parse URL
    SendError(400);
    return;
  }

  wpi::StringRef path;
  if (url.HasPath()) path = url.GetPath();
  wpi::errs() << "path: \"" << path << "\"\n";

  wpi::StringRef query;
  if (url.HasQuery()) query = url.GetQuery();
  wpi::errs() << "query: \"" << query << "\"\n";

  const bool isGET = m_request.GetMethod() == wpi::HTTP_GET;
  if (isGET && path.equals("/")) {
    // build HTML root page
    wpi::SmallString<256> buf;
    wpi::raw_svector_ostream os{buf};
    os << "<html><head><title>WebServer Example</title></head>";
    os << "<body><p>This is an example root page from the webserver.";
    os << "</body></html>";
    SendResponse(200, "OK", "text/html", os.str());
  } else {
    SendError(404, "Resource not found");
  }
}

int main() {
  // Kick off the event loop on a separate thread
  wpi::EventLoopRunner loop;
  loop.ExecAsync([](uv::Loop& loop) {
    auto tcp = uv::Tcp::Create(loop);

    // bind to listen address and port
    tcp->Bind("", 8080);

    // when we get a connection, accept it and start reading
    tcp->connection.connect([srv = tcp.get()] {
      auto tcp = srv->Accept();
      if (!tcp) return;
      wpi::errs() << "Got a connection\n";
      auto conn = std::make_shared<MyHttpServerConnection>(tcp);
      tcp->SetData(conn);
    });

    // start listening for incoming connections
    tcp->Listen();

    wpi::errs() << "Listening on port 8080\n";
  });

  // wait for a keypress to terminate
  std::getchar();
}
