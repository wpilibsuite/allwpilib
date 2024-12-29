// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdio>
#include <memory>

#include <wpi/print.h>

#include "wpinet/EventLoopRunner.h"
#include "wpinet/HttpServerConnection.h"
#include "wpinet/UrlParser.h"
#include "wpinet/uv/Loop.h"
#include "wpinet/uv/Tcp.h"

namespace uv = wpi::uv;

class MyHttpServerConnection : public wpi::HttpServerConnection {
 public:
  explicit MyHttpServerConnection(std::shared_ptr<uv::Stream> stream)
      : HttpServerConnection(stream) {}

 protected:
  void ProcessRequest() override;
};

void MyHttpServerConnection::ProcessRequest() {
  wpi::print(stderr, "HTTP request: '{}'\n", m_request.GetUrl());
  wpi::UrlParser url{m_request.GetUrl(),
                     m_request.GetMethod() == wpi::HTTP_CONNECT};
  if (!url.IsValid()) {
    // failed to parse URL
    SendError(400);
    return;
  }

  std::string_view path;
  if (url.HasPath()) {
    path = url.GetPath();
  }
  wpi::print(stderr, "path: \"{}\"\n", path);

  std::string_view query;
  if (url.HasQuery()) {
    query = url.GetQuery();
  }
  wpi::print(stderr, "query: \"{}\"\n", query);

  const bool isGET = m_request.GetMethod() == wpi::HTTP_GET;
  if (isGET && path == "/") {
    // build HTML root page
    SendResponse(200, "OK", "text/html",
                 "<html><head><title>WebServer Example</title></head>"
                 "<body><p>This is an example root page from the webserver."
                 "</body></html>");
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
      if (!tcp) {
        return;
      }
      std::fputs("Got a connection\n", stderr);
      auto conn = std::make_shared<MyHttpServerConnection>(tcp);
      tcp->SetData(conn);
    });

    // start listening for incoming connections
    tcp->Listen();

    std::fputs("Listening on port 8080\n", stderr);
  });

  // wait for a keypress to terminate
  static_cast<void>(std::getchar());
}
