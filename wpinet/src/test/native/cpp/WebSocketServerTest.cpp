// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// clang-format off
#include "wpi/net/WebSocket.hpp"
// clang-format on

#include <functional>
#include <memory>
#include <vector>

#include <catch2/generators/catch_generators.hpp>

#include "WebSocketTest.hpp"
#include "wpi/net/HttpParser.hpp"
#include "wpi/util/Base64.hpp"
#include "wpi/util/SmallString.hpp"
#include "wpi/util/sha1.hpp"

namespace wpi::net {

class WebSocketServerTest : public WebSocketTest {
 public:
  WebSocketServerTest() {
    resp.headersComplete.connect([this](bool) { headersDone = true; });

    serverPipe->Listen([this]() {
      auto conn = serverPipe->Accept();
      ws = WebSocket::CreateServer(*conn, "foo", "13");
      if (setupWebSocket) {
        setupWebSocket();
      }
    });
    clientPipe->Connect(pipeName, [this]() {
      clientPipe->StartRead();
      clientPipe->data.connect([this](uv::Buffer& buf, size_t size) {
        std::string_view data{buf.base, size};
        if (!headersDone) {
          data = resp.Execute(data);
          if (resp.HasError()) {
            Finish();
          }
          UNSCOPED_INFO(llhttp_errno_name(resp.GetError()));
          REQUIRE(resp.GetError() == HPE_PAUSED_UPGRADE);
          if (data.empty()) {
            return;
          }
        }
        wireData.insert(wireData.end(), data.begin(), data.end());
        if (handleData) {
          handleData(data);
        }
      });
      clientPipe->end.connect([this]() { Finish(); });
    });
  }

  std::function<void()> setupWebSocket;
  std::function<void(std::string_view)> handleData;
  std::vector<uint8_t> wireData;
  std::shared_ptr<WebSocket> ws;
  HttpParser resp{HttpParser::Type::RESPONSE};
  bool headersDone = false;
};

//
// Terminate closes the endpoint but doesn't send a close frame.
//

TEST_CASE_METHOD(WebSocketServerTest, "WebSocketServerTest Terminate",
                 "[websocket][server][terminate]") {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Terminate(); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1006);
    });
  };

  loop->Run();

  REQUIRE(wireData.empty());  // terminate doesn't send data
  REQUIRE(gotClosed == 1);
}

TEST_CASE_METHOD(WebSocketServerTest, "WebSocketServerTest TerminateCode",
                 "[websocket][server][terminate]") {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Terminate(1000); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1000);
    });
  };

  loop->Run();

  REQUIRE(wireData.empty());  // terminate doesn't send data
  REQUIRE(gotClosed == 1);
}

TEST_CASE_METHOD(WebSocketServerTest, "WebSocketServerTest TerminateReason",
                 "[websocket][server][terminate]") {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Terminate(1000, "reason"); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      REQUIRE(code == 1000);
      REQUIRE(reason == "reason");
    });
  };

  loop->Run();

  REQUIRE(wireData.empty());  // terminate doesn't send data
  REQUIRE(gotClosed == 1);
}

//
// Close() sends a close frame.
//

TEST_CASE_METHOD(WebSocketServerTest, "WebSocketServerTest CloseBasic",
                 "[websocket][server][close]") {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Close(); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1005);
    });
  };
  // need to respond with close for server to finish shutdown
  auto message = BuildMessage(0x08, true, true, {});
  handleData = [&](std::string_view) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  };

  loop->Run();

  auto expectData = BuildMessage(0x08, true, false, {});
  REQUIRE(wireData == expectData);
  REQUIRE(gotClosed == 1);
}

TEST_CASE_METHOD(WebSocketServerTest, "WebSocketServerTest CloseCode",
                 "[websocket][server][close]") {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Close(1000); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1000);
    });
  };
  // need to respond with close for server to finish shutdown
  const uint8_t contents[] = {0x03u, 0xe8u};
  auto message = BuildMessage(0x08, true, true, contents);
  handleData = [&](std::string_view) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  };

  loop->Run();

  auto expectData = BuildMessage(0x08, true, false, contents);
  REQUIRE(wireData == expectData);
  REQUIRE(gotClosed == 1);
}

TEST_CASE_METHOD(WebSocketServerTest, "WebSocketServerTest CloseReason",
                 "[websocket][server][close]") {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Close(1000, "hangup"); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      REQUIRE(code == 1000);
      REQUIRE(reason == "remote close: hangup");
    });
  };
  // need to respond with close for server to finish shutdown
  const uint8_t contents[] = {0x03u, 0xe8u, 'h', 'a', 'n', 'g', 'u', 'p'};
  auto message = BuildMessage(0x08, true, true, contents);
  handleData = [&](std::string_view) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  };

  loop->Run();

  auto expectData = BuildMessage(0x08, true, false, contents);
  REQUIRE(wireData == expectData);
  REQUIRE(gotClosed == 1);
}

//
// Receiving a close frame results in closure and echoing the close frame.
//

TEST_CASE_METHOD(WebSocketServerTest, "WebSocketServerTest ReceiveCloseBasic",
                 "[websocket][server][close]") {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1005);
    });
  };
  auto message = BuildMessage(0x08, true, true, {});
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  // the endpoint should echo the message
  auto expectData = BuildMessage(0x08, true, false, {});
  REQUIRE(wireData == expectData);
  REQUIRE(gotClosed == 1);
}

TEST_CASE_METHOD(WebSocketServerTest, "WebSocketServerTest ReceiveCloseCode",
                 "[websocket][server][close]") {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1000);
    });
  };
  const uint8_t contents[] = {0x03u, 0xe8u};
  auto message = BuildMessage(0x08, true, true, contents);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  // the endpoint should echo the message
  auto expectData = BuildMessage(0x08, true, false, contents);
  REQUIRE(wireData == expectData);
  REQUIRE(gotClosed == 1);
}

TEST_CASE_METHOD(WebSocketServerTest, "WebSocketServerTest ReceiveCloseReason",
                 "[websocket][server][close]") {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      REQUIRE(code == 1000);
      REQUIRE(reason == "remote close: hangup");
    });
  };
  const uint8_t contents[] = {0x03u, 0xe8u, 'h', 'a', 'n', 'g', 'u', 'p'};
  auto message = BuildMessage(0x08, true, true, contents);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  // the endpoint should echo the message
  auto expectData = BuildMessage(0x08, true, false, contents);
  REQUIRE(wireData == expectData);
  REQUIRE(gotClosed == 1);
}

//
// If an unknown opcode is received, the receiving endpoint MUST _Fail the
// WebSocket Connection_.
//

class WebSocketServerBadOpcodeTest : public WebSocketServerTest {};

TEST_CASE_METHOD(WebSocketServerBadOpcodeTest,
                 "WebSocketServerBadOpcodeTest Receive",
                 "[websocket][server][protocol]") {
  int gotCallback = 0;
  auto opcode =
      static_cast<uint8_t>(GENERATE(3, 4, 5, 6, 7, 0xb, 0xc, 0xd, 0xe, 0xf));
  std::vector<uint8_t> data(4, 0x03);
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1002);
    });
  };
  auto message = BuildMessage(opcode, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

//
// Control frames themselves MUST NOT be fragmented.
//

class WebSocketServerControlFrameTest : public WebSocketServerTest {};

TEST_CASE_METHOD(WebSocketServerControlFrameTest,
                 "WebSocketServerControlFrameTest ReceiveFragment",
                 "[websocket][server][control][fragment]") {
  int gotCallback = 0;
  auto opcode = static_cast<uint8_t>(GENERATE(0x8, 0x9, 0xa));
  std::vector<uint8_t> data(4, 0x03);
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1002);
    });
  };
  auto message = BuildMessage(opcode, false, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

//
// A fragmented message consists of a single frame with the FIN bit
// clear and an opcode other than 0, followed by zero or more frames
// with the FIN bit clear and the opcode set to 0, and terminated by
// a single frame with the FIN bit set and an opcode of 0.
//

// No previous message
TEST_CASE_METHOD(WebSocketServerTest,
                 "WebSocketServerTest ReceiveFragmentInvalidNoPrevFrame",
                 "[websocket][server][fragment][protocol]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(4, 0x03);
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1002);
    });
  };
  auto message = BuildMessage(0x00, false, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

// No previous message with FIN=1.
TEST_CASE_METHOD(WebSocketServerTest,
                 "WebSocketServerTest ReceiveFragmentInvalidNoPrevFragment",
                 "[websocket][server][fragment][protocol]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(4, 0x03);
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1002);
    });
  };
  auto message = BuildMessage(0x01, true, true, {});  // FIN=1
  auto message2 = BuildMessage(0x00, false, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}, {message2}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

// Incomplete fragment
TEST_CASE_METHOD(WebSocketServerTest,
                 "WebSocketServerTest ReceiveFragmentInvalidIncomplete",
                 "[websocket][server][fragment][protocol]") {
  int gotCallback = 0;
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1002);
    });
  };
  auto message = BuildMessage(0x01, false, true, {});
  auto message2 = BuildMessage(0x00, false, true, {});
  auto message3 = BuildMessage(0x01, true, true, {});
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}, {message2}, {message3}},
                      [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

// Normally fragments are combined into a single callback
TEST_CASE_METHOD(WebSocketServerTest, "WebSocketServerTest ReceiveFragment",
                 "[websocket][server][fragment]") {
  int gotCallback = 0;

  std::vector<uint8_t> data(4, 0x03);
  std::vector<uint8_t> data2(4, 0x04);
  std::vector<uint8_t> data3(4, 0x05);
  std::vector<uint8_t> combData{data};
#if __GNUC__ == 11
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overread"
#endif  // __GNUC__ == 11
  combData.insert(combData.end(), data2.begin(), data2.end());
#if __GNUC__ == 11
#pragma GCC diagnostic pop
#endif  // __GNUC__ == 11
  combData.insert(combData.end(), data3.begin(), data3.end());

  setupWebSocket = [&] {
    ws->binary.connect([&](auto inData, bool fin) {
      ++gotCallback;
      ws->Terminate();
      REQUIRE(fin);
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      REQUIRE(combData == recvData);
    });
  };

  auto message = BuildMessage(0x02, false, true, data);
  auto message2 = BuildMessage(0x00, false, true, data2);
  auto message3 = BuildMessage(0x00, true, true, data3);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}, {message2}, {message3}},
                      [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

// But can be configured for multiple callbacks
TEST_CASE_METHOD(WebSocketServerTest,
                 "WebSocketServerTest ReceiveFragmentSeparate",
                 "[websocket][server][fragment]") {
  int gotCallback = 0;

  std::vector<uint8_t> data(4, 0x03);
  std::vector<uint8_t> data2(4, 0x04);
  std::vector<uint8_t> data3(4, 0x05);
  std::vector<uint8_t> combData{data};
  combData.insert(combData.end(), data2.begin(), data2.end());
  combData.insert(combData.end(), data3.begin(), data3.end());

  setupWebSocket = [&] {
    ws->SetCombineFragments(false);
    ws->binary.connect([&](auto inData, bool fin) {
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      switch (++gotCallback) {
        case 1:
          REQUIRE_FALSE(fin);
          REQUIRE(data == recvData);
          break;
        case 2:
          REQUIRE_FALSE(fin);
          REQUIRE(data2 == recvData);
          break;
        case 3:
          ws->Terminate();
          REQUIRE(fin);
          REQUIRE(data3 == recvData);
          break;
        default:
          FAIL("too many callbacks");
          break;
      }
    });
  };

  auto message = BuildMessage(0x02, false, true, data);
  auto message2 = BuildMessage(0x00, false, true, data2);
  auto message3 = BuildMessage(0x00, true, true, data3);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}, {message2}, {message3}},
                      [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 3);
}

// Control frames can happen in the middle of a fragmented message
TEST_CASE_METHOD(WebSocketServerTest,
                 "WebSocketServerTest ReceiveFragmentWithControl",
                 "[websocket][server][fragment][control]") {
  int gotCallback = 0;
  int gotPongCallback = 0;

  std::vector<uint8_t> data(4, 0x03);
  std::vector<uint8_t> data2(4, 0x04);
  std::vector<uint8_t> data3(4, 0x05);
  std::vector<uint8_t> data4(4, 0x06);
  std::vector<uint8_t> combData{data};
#if __GNUC__ == 11
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overread"
#endif  // __GNUC__ == 11
  combData.insert(combData.end(), data2.begin(), data2.end());
#if __GNUC__ == 11
#pragma GCC diagnostic pop
#endif  // __GNUC__ == 11
  combData.insert(combData.end(), data4.begin(), data4.end());

  setupWebSocket = [&] {
    ws->binary.connect([&](auto inData, bool fin) {
      REQUIRE(gotPongCallback);
      ++gotCallback;
      ws->Terminate();
      REQUIRE(fin);
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      REQUIRE(combData == recvData);
    });
    ws->pong.connect([&](auto inData) {
      REQUIRE_FALSE(gotCallback);
      ++gotPongCallback;
    });
  };

  auto message = BuildMessage(0x02, false, true, data);
  auto message2 = BuildMessage(0x00, false, true, data2);
  auto message3 = BuildMessage(0x0a, true, true, data3);
  auto message4 = BuildMessage(0x00, true, true, data4);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}, {message2}, {message3}, {message4}},
                      [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
  REQUIRE(gotPongCallback == 1);
}

//
// Maximum message size is limited.
//

// Single message
TEST_CASE_METHOD(WebSocketServerTest, "WebSocketServerTest ReceiveTooLarge",
                 "[websocket][server][limits]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(2048, 0x03u);
  setupWebSocket = [&] {
    ws->SetMaxMessageSize(1024);
    ws->binary.connect([&](auto, bool) {
      ws->Terminate();
      FAIL("Should not have gotten unmasked message");
    });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1009);
    });
  };
  auto message = BuildMessage(0x01, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

// Applied across fragments if combining
TEST_CASE_METHOD(WebSocketServerTest,
                 "WebSocketServerTest ReceiveTooLargeFragmented",
                 "[websocket][server][limits][fragment]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(768, 0x03u);
  setupWebSocket = [&] {
    ws->SetMaxMessageSize(1024);
    ws->binary.connect([&](auto, bool) {
      ws->Terminate();
      FAIL("Should not have gotten unmasked message");
    });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1009);
    });
  };
  auto message = BuildMessage(0x01, false, true, data);
  auto message2 = BuildMessage(0x00, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}, {message2}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

//
// Send and receive data.
//

class WebSocketServerDataTest : public WebSocketServerTest {};

TEST_CASE_METHOD(WebSocketServerDataTest, "WebSocketServerDataTest SendText",
                 "[websocket][server][data]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(GENERATE(size_t{0}, size_t{1}, size_t{125},
                                     size_t{126}, size_t{65535}, size_t{65536}),
                            ' ');
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) {
      ws->SendText({{data}}, [&](auto bufs, uv::Error) {
        ++gotCallback;
        ws->Terminate();
        REQUIRE_FALSE(bufs.empty());
        REQUIRE(bufs[0].base == reinterpret_cast<const char*>(data.data()));
      });
    });
  };

  loop->Run();

  auto expectData = BuildMessage(0x01, true, false, data);
  REQUIRE(wireData == expectData);
  REQUIRE(gotCallback == 1);
}

TEST_CASE_METHOD(WebSocketServerDataTest, "WebSocketServerDataTest SendBinary",
                 "[websocket][server][data]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(GENERATE(size_t{0}, size_t{1}, size_t{125},
                                     size_t{126}, size_t{65535}, size_t{65536}),
                            0x03u);
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) {
      ws->SendBinary({{data}}, [&](auto bufs, uv::Error) {
        ++gotCallback;
        ws->Terminate();
        REQUIRE_FALSE(bufs.empty());
        REQUIRE(bufs[0].base == reinterpret_cast<const char*>(data.data()));
      });
    });
  };

  loop->Run();

  auto expectData = BuildMessage(0x02, true, false, data);
  REQUIRE(wireData == expectData);
  REQUIRE(gotCallback == 1);
}

TEST_CASE_METHOD(WebSocketServerDataTest, "WebSocketServerDataTest SendPing",
                 "[websocket][server][control]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(GENERATE(size_t{0}, size_t{1}, size_t{125},
                                     size_t{126}, size_t{65535}, size_t{65536}),
                            0x03u);
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) {
      ws->SendPing({{data}}, [&](auto bufs, uv::Error) {
        ++gotCallback;
        ws->Terminate();
        REQUIRE_FALSE(bufs.empty());
        REQUIRE(bufs[0].base == reinterpret_cast<const char*>(data.data()));
      });
    });
  };

  loop->Run();

  auto expectData = BuildMessage(0x09, true, false, data);
  REQUIRE(wireData == expectData);
  REQUIRE(gotCallback == 1);
}

TEST_CASE_METHOD(WebSocketServerDataTest, "WebSocketServerDataTest SendPong",
                 "[websocket][server][control]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(GENERATE(size_t{0}, size_t{1}, size_t{125},
                                     size_t{126}, size_t{65535}, size_t{65536}),
                            0x03u);
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) {
      ws->SendPong({{data}}, [&](auto bufs, uv::Error) {
        ++gotCallback;
        ws->Terminate();
        REQUIRE_FALSE(bufs.empty());
        REQUIRE(bufs[0].base == reinterpret_cast<const char*>(data.data()));
      });
    });
  };

  loop->Run();

  auto expectData = BuildMessage(0x0a, true, false, data);
  REQUIRE(wireData == expectData);
  REQUIRE(gotCallback == 1);
}

TEST_CASE_METHOD(WebSocketServerDataTest, "WebSocketServerDataTest ReceiveText",
                 "[websocket][server][data]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(GENERATE(size_t{0}, size_t{1}, size_t{125},
                                     size_t{126}, size_t{65535}, size_t{65536}),
                            ' ');
  setupWebSocket = [&] {
    ws->text.connect([&](std::string_view inData, bool fin) {
      ++gotCallback;
      ws->Terminate();
      REQUIRE(fin);
      std::vector<uint8_t> recvData;
      recvData.insert(recvData.end(), inData.begin(), inData.end());
      REQUIRE(data == recvData);
    });
  };
  auto message = BuildMessage(0x01, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

TEST_CASE_METHOD(WebSocketServerDataTest,
                 "WebSocketServerDataTest ReceiveBinary",
                 "[websocket][server][data]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(GENERATE(size_t{0}, size_t{1}, size_t{125},
                                     size_t{126}, size_t{65535}, size_t{65536}),
                            0x03u);
  setupWebSocket = [&] {
    ws->binary.connect([&](auto inData, bool fin) {
      ++gotCallback;
      ws->Terminate();
      REQUIRE(fin);
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      REQUIRE(data == recvData);
    });
  };
  auto message = BuildMessage(0x02, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

TEST_CASE_METHOD(WebSocketServerDataTest, "WebSocketServerDataTest ReceivePing",
                 "[websocket][server][control]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(GENERATE(size_t{0}, size_t{1}, size_t{125},
                                     size_t{126}, size_t{65535}, size_t{65536}),
                            0x03u);
  setupWebSocket = [&] {
    ws->ping.connect([&](auto inData) {
      ++gotCallback;
      ws->Terminate();
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      REQUIRE(data == recvData);
    });
  };
  auto message = BuildMessage(0x09, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

TEST_CASE_METHOD(WebSocketServerDataTest, "WebSocketServerDataTest ReceivePong",
                 "[websocket][server][control]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(GENERATE(size_t{0}, size_t{1}, size_t{125},
                                     size_t{126}, size_t{65535}, size_t{65536}),
                            0x03u);
  setupWebSocket = [&] {
    ws->pong.connect([&](auto inData) {
      ++gotCallback;
      ws->Terminate();
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      REQUIRE(data == recvData);
    });
  };
  auto message = BuildMessage(0x0a, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

//
// The server must close the connection if an unmasked frame is received.
//

TEST_CASE_METHOD(WebSocketServerDataTest,
                 "WebSocketServerDataTest ReceiveUnmasked",
                 "[websocket][server][data][protocol]") {
  int gotCallback = 0;
  std::vector<uint8_t> data(GENERATE(size_t{0}, size_t{1}, size_t{125},
                                     size_t{126}, size_t{65535}, size_t{65536}),
                            ' ');
  setupWebSocket = [&] {
    ws->text.connect([&](std::string_view, bool) {
      ws->Terminate();
      FAIL("Should not have gotten unmasked message");
    });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      UNSCOPED_INFO("reason: " << reason);
      REQUIRE(code == 1002);
    });
  };
  auto message = BuildMessage(0x01, true, false, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  REQUIRE(gotCallback == 1);
}

}  // namespace wpi::net
