// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/WebSocketServer.h"  // NOLINT(build/include_order)

#include <vector>

#include <wpi/SmallString.h>

#include "WebSocketTest.h"

namespace wpi {

class WebSocketIntegrationTest : public WebSocketTest {};

TEST_F(WebSocketIntegrationTest, Open) {
  int gotServerOpen = 0;
  int gotClientOpen = 0;

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto server = WebSocketServer::Create(*conn);
    server->connected.connect([&](std::string_view url, WebSocket&) {
      ++gotServerOpen;
      ASSERT_EQ(url, "/test");
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      Finish();
      if (code != 1005 && code != 1006) {
        FAIL() << "Code: " << code << " Reason: " << reason;
      }
    });
    ws->open.connect([&, s = ws.get()](std::string_view) {
      ++gotClientOpen;
      s->Close();
    });
  });

  loop->Run();

  ASSERT_EQ(gotServerOpen, 1);
  ASSERT_EQ(gotClientOpen, 1);
}

TEST_F(WebSocketIntegrationTest, Protocol) {
  int gotServerOpen = 0;
  int gotClientOpen = 0;

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto server = WebSocketServer::Create(*conn, {"proto1", "proto2"});
    server->connected.connect([&](std::string_view, WebSocket& ws) {
      ++gotServerOpen;
      ASSERT_EQ(ws.GetProtocol(), "proto1");
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws =
        WebSocket::CreateClient(*clientPipe, "/test", pipeName, {"proto1"});
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      Finish();
      if (code != 1005 && code != 1006) {
        FAIL() << "Code: " << code << " Reason: " << reason;
      }
    });
    ws->open.connect([&, s = ws.get()](std::string_view protocol) {
      ++gotClientOpen;
      s->Close();
      ASSERT_EQ(protocol, "proto1");
    });
  });

  loop->Run();

  ASSERT_EQ(gotServerOpen, 1);
  ASSERT_EQ(gotClientOpen, 1);
}

TEST_F(WebSocketIntegrationTest, ServerSendBinary) {
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
        FAIL() << "Code: " << code << " Reason: " << reason;
      }
    });
    ws->binary.connect([&](auto data, bool) {
      ++gotData;
      std::vector<uint8_t> recvData{data.begin(), data.end()};
      std::vector<uint8_t> expectData{0x03, 0x04};
      ASSERT_EQ(recvData, expectData);
    });
  });

  loop->Run();

  ASSERT_EQ(gotData, 1);
}

TEST_F(WebSocketIntegrationTest, ClientSendText) {
  int gotData = 0;

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto server = WebSocketServer::Create(*conn);
    server->connected.connect([&](std::string_view, WebSocket& ws) {
      ws.text.connect([&](std::string_view data, bool) {
        ++gotData;
        ASSERT_EQ(data, "hello");
      });
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      Finish();
      if (code != 1005 && code != 1006) {
        FAIL() << "Code: " << code << " Reason: " << reason;
      }
    });
    ws->open.connect([&, s = ws.get()](std::string_view) {
      s->SendText({{"hello"}}, [&](auto, uv::Error) {});
      s->Close();
    });
  });

  loop->Run();

  ASSERT_EQ(gotData, 1);
}

TEST_F(WebSocketIntegrationTest, ServerSendPing) {
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
        ASSERT_EQ(recvData, expectData);
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
        FAIL() << "Code: " << code << " Reason: " << reason;
      }
    });
    ws->ping.connect([&](auto data) {
      ++gotPing;
      std::vector<uint8_t> recvData{data.begin(), data.end()};
      std::vector<uint8_t> expectData{0x03, 0x04};
      ASSERT_EQ(recvData, expectData);
    });
    ws->text.connect([&](std::string_view data, bool) {
      ++gotData;
      ASSERT_EQ(data, "hello");
    });
  });

  loop->Run();

  ASSERT_EQ(gotPing, 2);
  ASSERT_EQ(gotPong, 2);
  ASSERT_EQ(gotData, 2);
}

}  // namespace wpi
