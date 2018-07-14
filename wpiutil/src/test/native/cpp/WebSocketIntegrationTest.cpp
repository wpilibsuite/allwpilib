/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/WebSocketServer.h"  // NOLINT(build/include_order)

#include "WebSocketTest.h"
#include "wpi/HttpParser.h"
#include "wpi/SmallString.h"

namespace wpi {

class WebSocketIntegrationTest : public WebSocketTest {};

TEST_F(WebSocketIntegrationTest, Open) {
  int gotServerOpen = 0;
  int gotClientOpen = 0;

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto server = WebSocketServer::Create(*conn);
    server->connected.connect([&](StringRef url, WebSocket&) {
      ++gotServerOpen;
      ASSERT_EQ(url, "/test");
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, StringRef reason) {
      Finish();
      if (code != 1005 && code != 1006)
        FAIL() << "Code: " << code << " Reason: " << reason;
    });
    ws->open.connect([&, s = ws.get() ](StringRef) {
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
    server->connected.connect([&](StringRef, WebSocket& ws) {
      ++gotServerOpen;
      ASSERT_EQ(ws.GetProtocol(), "proto1");
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws =
        WebSocket::CreateClient(*clientPipe, "/test", pipeName, {"proto1"});
    ws->closed.connect([&](uint16_t code, StringRef reason) {
      Finish();
      if (code != 1005 && code != 1006)
        FAIL() << "Code: " << code << " Reason: " << reason;
    });
    ws->open.connect([&, s = ws.get() ](StringRef protocol) {
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
    server->connected.connect([&](StringRef, WebSocket& ws) {
      ws.SendBinary(uv::Buffer{"\x03\x04", 2}, [&](auto, uv::Error) {});
      ws.Close();
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, StringRef reason) {
      Finish();
      if (code != 1005 && code != 1006)
        FAIL() << "Code: " << code << " Reason: " << reason;
    });
    ws->binary.connect([&](ArrayRef<uint8_t> data, bool) {
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
    server->connected.connect([&](StringRef, WebSocket& ws) {
      ws.text.connect([&](StringRef data, bool) {
        ++gotData;
        ASSERT_EQ(data, "hello");
      });
    });
  });

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, StringRef reason) {
      Finish();
      if (code != 1005 && code != 1006)
        FAIL() << "Code: " << code << " Reason: " << reason;
    });
    ws->open.connect([&, s = ws.get() ](StringRef) {
      s->SendText(uv::Buffer{"hello"}, [&](auto, uv::Error) {});
      s->Close();
    });
  });

  loop->Run();

  ASSERT_EQ(gotData, 1);
}

}  // namespace wpi
