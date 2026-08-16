// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/HttpWebSocketServerConnection.hpp"

#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "WebSocketTest.hpp"
#include "wpi/net/uv/Timer.hpp"

namespace wpi::net {

class HttpWebSocketServerConnectionTest
    : public HttpWebSocketServerConnection<HttpWebSocketServerConnectionTest> {
 public:
  HttpWebSocketServerConnectionTest(std::shared_ptr<uv::Stream> stream,
                                    std::span<const std::string_view> protocols,
                                    uv::Timer::Time requestTimeout = {})
      : HttpWebSocketServerConnection{stream, protocols, requestTimeout} {}

  void ProcessRequest() override { ++gotRequest; }
  void ProcessWsUpgrade() override { ++gotUpgrade; }

  int gotRequest = 0;
  int gotUpgrade = 0;
};

TEST_CASE_METHOD(WebSocketTest,
                 "HttpWebSocketServerConnection times out partial request",
                 "[http][websocket][server]") {
  std::shared_ptr<HttpWebSocketServerConnectionTest> connection;
  bool gotEnd = false;
  serverPipe->Listen([&] {
    auto stream = serverPipe->Accept();
    connection = std::make_shared<HttpWebSocketServerConnectionTest>(
        stream, std::span<const std::string_view>{}, uv::Timer::Time{10});
    stream->SetData(connection);
  });
  clientPipe->end.connect([&] {
    gotEnd = true;
    Finish();
  });
  clientPipe->Connect(pipeName, [&] {
    clientPipe->StartRead();
    clientPipe->Write({{"GET / HTTP/1.1\r\n"}}, [](auto, uv::Error) {});
  });

  loop->Run();

  CHECK(gotEnd);
  REQUIRE(connection);
  CHECK(connection->gotRequest == 0);
  CHECK(connection->gotUpgrade == 0);
}

}  // namespace wpi::net
