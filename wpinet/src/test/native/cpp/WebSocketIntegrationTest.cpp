// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// clang-format off
#include "wpi/net/WebSocketServer.hpp"
// clang-format on

#include <vector>

#include "WebSocketTest.hpp"
#include "wpi/util/SmallString.hpp"

namespace wpi::net {

class WebSocketIntegrationTest : public WebSocketTest {};

TEST_CASE_METHOD(WebSocketIntegrationTest, "WebSocketIntegrationTest Open",
                 "[websocket][integration][handshake]") {
  int gotServerOpen = 0;
  int gotClientOpen = 0;

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto server = WebSocketServer::Create(*conn);
    server->connected.connect([&](std::string_view url, WebSocket&) {
      ++gotServerOpen;
      REQUIRE(url == "/test");
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      Finish();
      if (code != 1005 && code != 1006) {
        FAIL("Code: " << code << " Reason: " << reason);
      }
    });
    ws->open.connect([&, s = ws.get()](std::string_view) {
      ++gotClientOpen;
      s->Close();
    });
  });

  loop->Run();

  REQUIRE(gotServerOpen == 1);
  REQUIRE(gotClientOpen == 1);
}

TEST_CASE_METHOD(WebSocketIntegrationTest, "WebSocketIntegrationTest Protocol",
                 "[websocket][integration][protocol]") {
  int gotServerOpen = 0;
  int gotClientOpen = 0;

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto server = WebSocketServer::Create(*conn, {"proto1", "proto2"});
    server->connected.connect([&](std::string_view, WebSocket& ws) {
      ++gotServerOpen;
      REQUIRE(ws.GetProtocol() == "proto1");
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws =
        WebSocket::CreateClient(*clientPipe, "/test", pipeName, {"proto1"});
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      Finish();
      if (code != 1005 && code != 1006) {
        FAIL("Code: " << code << " Reason: " << reason);
      }
    });
    ws->open.connect([&, s = ws.get()](std::string_view protocol) {
      ++gotClientOpen;
      s->Close();
      REQUIRE(protocol == "proto1");
    });
  });

  loop->Run();

  REQUIRE(gotServerOpen == 1);
  REQUIRE(gotClientOpen == 1);
}

TEST_CASE_METHOD(WebSocketIntegrationTest,
                 "WebSocketIntegrationTest ServerSendBinary",
                 "[websocket][integration][data]") {
  int gotData = 0;

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto server = WebSocketServer::Create(*conn);
    server->connected.connect([&](std::string_view, WebSocket& ws) {
      ws.SendBinary({uv::Buffer{"\x03\x04", 2}}, [&](auto, uv::Error) {});
      ws.Close();
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      Finish();
      if (code != 1005 && code != 1006) {
        FAIL("Code: " << code << " Reason: " << reason);
      }
    });
    ws->binary.connect([&](auto data, bool) {
      ++gotData;
      std::vector<uint8_t> recvData{data.begin(), data.end()};
      std::vector<uint8_t> expectData{0x03, 0x04};
      REQUIRE(recvData == expectData);
    });
  });

  loop->Run();

  REQUIRE(gotData == 1);
}

TEST_CASE_METHOD(WebSocketIntegrationTest,
                 "WebSocketIntegrationTest ClientSendText",
                 "[websocket][integration][data]") {
  int gotData = 0;

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto server = WebSocketServer::Create(*conn);
    server->connected.connect([&](std::string_view, WebSocket& ws) {
      ws.text.connect([&](std::string_view data, bool) {
        ++gotData;
        REQUIRE(data == "hello");
      });
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      Finish();
      if (code != 1005 && code != 1006) {
        FAIL("Code: " << code << " Reason: " << reason);
      }
    });
    ws->open.connect([&, s = ws.get()](std::string_view) {
      s->SendText({{"hello"}}, [&](auto, uv::Error) {});
      s->Close();
    });
  });

  loop->Run();

  REQUIRE(gotData == 1);
}

TEST_CASE_METHOD(WebSocketIntegrationTest,
                 "WebSocketIntegrationTest ServerSendPing",
                 "[websocket][integration][control]") {
  int gotPing = 0;
  int gotPong = 0;
  int gotData = 0;

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto server = WebSocketServer::Create(*conn);
    server->connected.connect([&](std::string_view, WebSocket& ws) {
      ws.SendText({{"hello"}}, [&](auto, uv::Error) {});
      ws.SendPing({uv::Buffer{"\x03\x04", 2}}, [&](auto, uv::Error) {});
      ws.SendPing({uv::Buffer{"\x03\x04", 2}}, [&](auto, uv::Error) {});
      ws.SendText({{"hello"}}, [&](auto, uv::Error) {});
      ws.pong.connect([&](auto data) {
        ++gotPong;
        std::vector<uint8_t> recvData{data.begin(), data.end()};
        std::vector<uint8_t> expectData{0x03, 0x04};
        REQUIRE(recvData == expectData);
        if (gotPong == 2) {
          ws.Close();
        }
      });
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      Finish();
      if (code != 1005 && code != 1006) {
        FAIL("Code: " << code << " Reason: " << reason);
      }
    });
    ws->ping.connect([&](auto data) {
      ++gotPing;
      std::vector<uint8_t> recvData{data.begin(), data.end()};
      std::vector<uint8_t> expectData{0x03, 0x04};
      REQUIRE(recvData == expectData);
    });
    ws->text.connect([&](std::string_view data, bool) {
      ++gotData;
      REQUIRE(data == "hello");
    });
  });

  loop->Run();

  REQUIRE(gotPing == 2);
  REQUIRE(gotPong == 2);
  REQUIRE(gotData == 2);
}

}  // namespace wpi::net
