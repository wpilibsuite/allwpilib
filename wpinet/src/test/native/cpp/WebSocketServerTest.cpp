// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/WebSocket.h"  // NOLINT(build/include_order)

#include <functional>
#include <memory>
#include <vector>

#include <wpi/Base64.h>
#include <wpi/SmallString.h>
#include <wpi/sha1.h>

#include "WebSocketTest.h"
#include "wpinet/HttpParser.h"

namespace wpi {

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
          ASSERT_EQ(resp.GetError(), HPE_OK)
              << http_errno_name(resp.GetError());
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
  HttpParser resp{HttpParser::RESPONSE};
  bool headersDone = false;
};

//
// Terminate closes the endpoint but doesn't send a close frame.
//

TEST_F(WebSocketServerTest, Terminate) {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Terminate(); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      ASSERT_EQ(code, 1006) << "reason: " << reason;
    });
  };

  loop->Run();

  ASSERT_TRUE(wireData.empty());  // terminate doesn't send data
  ASSERT_EQ(gotClosed, 1);
}

TEST_F(WebSocketServerTest, TerminateCode) {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Terminate(1000); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      ASSERT_EQ(code, 1000) << "reason: " << reason;
    });
  };

  loop->Run();

  ASSERT_TRUE(wireData.empty());  // terminate doesn't send data
  ASSERT_EQ(gotClosed, 1);
}

TEST_F(WebSocketServerTest, TerminateReason) {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Terminate(1000, "reason"); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      ASSERT_EQ(code, 1000);
      ASSERT_EQ(reason, "reason");
    });
  };

  loop->Run();

  ASSERT_TRUE(wireData.empty());  // terminate doesn't send data
  ASSERT_EQ(gotClosed, 1);
}

//
// Close() sends a close frame.
//

TEST_F(WebSocketServerTest, CloseBasic) {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Close(); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      ASSERT_EQ(code, 1005) << "reason: " << reason;
    });
  };
  // need to respond with close for server to finish shutdown
  auto message = BuildMessage(0x08, true, true, {});
  handleData = [&](std::string_view) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  };

  loop->Run();

  auto expectData = BuildMessage(0x08, true, false, {});
  ASSERT_EQ(wireData, expectData);
  ASSERT_EQ(gotClosed, 1);
}

TEST_F(WebSocketServerTest, CloseCode) {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Close(1000); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      ASSERT_EQ(code, 1000) << "reason: " << reason;
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
  ASSERT_EQ(wireData, expectData);
  ASSERT_EQ(gotClosed, 1);
}

TEST_F(WebSocketServerTest, CloseReason) {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) { ws->Close(1000, "hangup"); });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      ASSERT_EQ(code, 1000);
      ASSERT_EQ(reason, "remote close: hangup");
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
  ASSERT_EQ(wireData, expectData);
  ASSERT_EQ(gotClosed, 1);
}

//
// Receiving a close frame results in closure and echoing the close frame.
//

TEST_F(WebSocketServerTest, ReceiveCloseBasic) {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      ASSERT_EQ(code, 1005) << "reason: " << reason;
    });
  };
  auto message = BuildMessage(0x08, true, true, {});
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  // the endpoint should echo the message
  auto expectData = BuildMessage(0x08, true, false, {});
  ASSERT_EQ(wireData, expectData);
  ASSERT_EQ(gotClosed, 1);
}

TEST_F(WebSocketServerTest, ReceiveCloseCode) {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      ASSERT_EQ(code, 1000) << "reason: " << reason;
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
  ASSERT_EQ(wireData, expectData);
  ASSERT_EQ(gotClosed, 1);
}

TEST_F(WebSocketServerTest, ReceiveCloseReason) {
  int gotClosed = 0;
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotClosed;
      ASSERT_EQ(code, 1000);
      ASSERT_EQ(reason, "remote close: hangup");
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
  ASSERT_EQ(wireData, expectData);
  ASSERT_EQ(gotClosed, 1);
}

//
// If an unknown opcode is received, the receiving endpoint MUST _Fail the
// WebSocket Connection_.
//

class WebSocketServerBadOpcodeTest
    : public WebSocketServerTest,
      public ::testing::WithParamInterface<uint8_t> {};

INSTANTIATE_TEST_SUITE_P(WebSocketServerBadOpcodeTests,
                         WebSocketServerBadOpcodeTest,
                         ::testing::Values(3, 4, 5, 6, 7, 0xb, 0xc, 0xd, 0xe,
                                           0xf));

TEST_P(WebSocketServerBadOpcodeTest, Receive) {
  int gotCallback = 0;
  std::vector<uint8_t> data(4, 0x03);
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      ASSERT_EQ(code, 1002) << "reason: " << reason;
    });
  };
  auto message = BuildMessage(GetParam(), true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

//
// Control frames themselves MUST NOT be fragmented.
//

class WebSocketServerControlFrameTest
    : public WebSocketServerTest,
      public ::testing::WithParamInterface<uint8_t> {};

INSTANTIATE_TEST_SUITE_P(WebSocketServerControlFrameTests,
                         WebSocketServerControlFrameTest,
                         ::testing::Values(0x8, 0x9, 0xa));

TEST_P(WebSocketServerControlFrameTest, ReceiveFragment) {
  int gotCallback = 0;
  std::vector<uint8_t> data(4, 0x03);
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      ASSERT_EQ(code, 1002) << "reason: " << reason;
    });
  };
  auto message = BuildMessage(GetParam(), false, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

//
// A fragmented message consists of a single frame with the FIN bit
// clear and an opcode other than 0, followed by zero or more frames
// with the FIN bit clear and the opcode set to 0, and terminated by
// a single frame with the FIN bit set and an opcode of 0.
//

// No previous message
TEST_F(WebSocketServerTest, ReceiveFragmentInvalidNoPrevFrame) {
  int gotCallback = 0;
  std::vector<uint8_t> data(4, 0x03);
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      ASSERT_EQ(code, 1002) << "reason: " << reason;
    });
  };
  auto message = BuildMessage(0x00, false, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

// No previous message with FIN=1.
TEST_F(WebSocketServerTest, ReceiveFragmentInvalidNoPrevFragment) {
  int gotCallback = 0;
  std::vector<uint8_t> data(4, 0x03);
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      ASSERT_EQ(code, 1002) << "reason: " << reason;
    });
  };
  auto message = BuildMessage(0x01, true, true, {});  // FIN=1
  auto message2 = BuildMessage(0x00, false, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}, {message2}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

// Incomplete fragment
TEST_F(WebSocketServerTest, ReceiveFragmentInvalidIncomplete) {
  int gotCallback = 0;
  setupWebSocket = [&] {
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      ASSERT_EQ(code, 1002) << "reason: " << reason;
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

  ASSERT_EQ(gotCallback, 1);
}

// Normally fragments are combined into a single callback
TEST_F(WebSocketServerTest, ReceiveFragment) {
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
      ASSERT_TRUE(fin);
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      ASSERT_EQ(combData, recvData);
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

  ASSERT_EQ(gotCallback, 1);
}

// But can be configured for multiple callbacks
TEST_F(WebSocketServerTest, ReceiveFragmentSeparate) {
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
          ASSERT_FALSE(fin);
          ASSERT_EQ(data, recvData);
          break;
        case 2:
          ASSERT_FALSE(fin);
          ASSERT_EQ(data2, recvData);
          break;
        case 3:
          ws->Terminate();
          ASSERT_TRUE(fin);
          ASSERT_EQ(data3, recvData);
          break;
        default:
          FAIL() << "too many callbacks";
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

  ASSERT_EQ(gotCallback, 3);
}

// Control frames can happen in the middle of a fragmented message
TEST_F(WebSocketServerTest, ReceiveFragmentWithControl) {
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
      ASSERT_TRUE(gotPongCallback);
      ++gotCallback;
      ws->Terminate();
      ASSERT_TRUE(fin);
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      ASSERT_EQ(combData, recvData);
    });
    ws->pong.connect([&](auto inData) {
      ASSERT_FALSE(gotCallback);
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

  ASSERT_EQ(gotCallback, 1);
  ASSERT_EQ(gotPongCallback, 1);
}

//
// Maximum message size is limited.
//

// Single message
TEST_F(WebSocketServerTest, ReceiveTooLarge) {
  int gotCallback = 0;
  std::vector<uint8_t> data(2048, 0x03u);
  setupWebSocket = [&] {
    ws->SetMaxMessageSize(1024);
    ws->binary.connect([&](auto, bool) {
      ws->Terminate();
      FAIL() << "Should not have gotten unmasked message";
    });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      ASSERT_EQ(code, 1009) << "reason: " << reason;
    });
  };
  auto message = BuildMessage(0x01, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

// Applied across fragments if combining
TEST_F(WebSocketServerTest, ReceiveTooLargeFragmented) {
  int gotCallback = 0;
  std::vector<uint8_t> data(768, 0x03u);
  setupWebSocket = [&] {
    ws->SetMaxMessageSize(1024);
    ws->binary.connect([&](auto, bool) {
      ws->Terminate();
      FAIL() << "Should not have gotten unmasked message";
    });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      ASSERT_EQ(code, 1009) << "reason: " << reason;
    });
  };
  auto message = BuildMessage(0x01, false, true, data);
  auto message2 = BuildMessage(0x00, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}, {message2}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

//
// Send and receive data.
//

class WebSocketServerDataTest : public WebSocketServerTest,
                                public ::testing::WithParamInterface<size_t> {};

INSTANTIATE_TEST_SUITE_P(WebSocketServerDataTests, WebSocketServerDataTest,
                         ::testing::Values(0, 1, 125, 126, 65535, 65536));

TEST_P(WebSocketServerDataTest, SendText) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), ' ');
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) {
      ws->SendText({{data}}, [&](auto bufs, uv::Error) {
        ++gotCallback;
        ws->Terminate();
        ASSERT_FALSE(bufs.empty());
        ASSERT_EQ(bufs[0].base, reinterpret_cast<const char*>(data.data()));
      });
    });
  };

  loop->Run();

  auto expectData = BuildMessage(0x01, true, false, data);
  ASSERT_EQ(wireData, expectData);
  ASSERT_EQ(gotCallback, 1);
}

TEST_P(WebSocketServerDataTest, SendBinary) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), 0x03u);
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) {
      ws->SendBinary({{data}}, [&](auto bufs, uv::Error) {
        ++gotCallback;
        ws->Terminate();
        ASSERT_FALSE(bufs.empty());
        ASSERT_EQ(bufs[0].base, reinterpret_cast<const char*>(data.data()));
      });
    });
  };

  loop->Run();

  auto expectData = BuildMessage(0x02, true, false, data);
  ASSERT_EQ(wireData, expectData);
  ASSERT_EQ(gotCallback, 1);
}

TEST_P(WebSocketServerDataTest, SendPing) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), 0x03u);
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) {
      ws->SendPing({{data}}, [&](auto bufs, uv::Error) {
        ++gotCallback;
        ws->Terminate();
        ASSERT_FALSE(bufs.empty());
        ASSERT_EQ(bufs[0].base, reinterpret_cast<const char*>(data.data()));
      });
    });
  };

  loop->Run();

  auto expectData = BuildMessage(0x09, true, false, data);
  ASSERT_EQ(wireData, expectData);
  ASSERT_EQ(gotCallback, 1);
}

TEST_P(WebSocketServerDataTest, SendPong) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), 0x03u);
  setupWebSocket = [&] {
    ws->open.connect([&](std::string_view) {
      ws->SendPong({{data}}, [&](auto bufs, uv::Error) {
        ++gotCallback;
        ws->Terminate();
        ASSERT_FALSE(bufs.empty());
        ASSERT_EQ(bufs[0].base, reinterpret_cast<const char*>(data.data()));
      });
    });
  };

  loop->Run();

  auto expectData = BuildMessage(0x0a, true, false, data);
  ASSERT_EQ(wireData, expectData);
  ASSERT_EQ(gotCallback, 1);
}

TEST_P(WebSocketServerDataTest, ReceiveText) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), ' ');
  setupWebSocket = [&] {
    ws->text.connect([&](std::string_view inData, bool fin) {
      ++gotCallback;
      ws->Terminate();
      ASSERT_TRUE(fin);
      std::vector<uint8_t> recvData;
      recvData.insert(recvData.end(), inData.begin(), inData.end());
      ASSERT_EQ(data, recvData);
    });
  };
  auto message = BuildMessage(0x01, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

TEST_P(WebSocketServerDataTest, ReceiveBinary) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), 0x03u);
  setupWebSocket = [&] {
    ws->binary.connect([&](auto inData, bool fin) {
      ++gotCallback;
      ws->Terminate();
      ASSERT_TRUE(fin);
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      ASSERT_EQ(data, recvData);
    });
  };
  auto message = BuildMessage(0x02, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

TEST_P(WebSocketServerDataTest, ReceivePing) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), 0x03u);
  setupWebSocket = [&] {
    ws->ping.connect([&](auto inData) {
      ++gotCallback;
      ws->Terminate();
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      ASSERT_EQ(data, recvData);
    });
  };
  auto message = BuildMessage(0x09, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

TEST_P(WebSocketServerDataTest, ReceivePong) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), 0x03u);
  setupWebSocket = [&] {
    ws->pong.connect([&](auto inData) {
      ++gotCallback;
      ws->Terminate();
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      ASSERT_EQ(data, recvData);
    });
  };
  auto message = BuildMessage(0x0a, true, true, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

//
// The server must close the connection if an unmasked frame is received.
//

TEST_P(WebSocketServerDataTest, ReceiveUnmasked) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), ' ');
  setupWebSocket = [&] {
    ws->text.connect([&](std::string_view, bool) {
      ws->Terminate();
      FAIL() << "Should not have gotten unmasked message";
    });
    ws->closed.connect([&](uint16_t code, std::string_view reason) {
      ++gotCallback;
      ASSERT_EQ(code, 1002) << "reason: " << reason;
    });
  };
  auto message = BuildMessage(0x01, true, false, data);
  resp.headersComplete.connect([&](bool) {
    clientPipe->Write({{message}}, [&](auto bufs, uv::Error) {});
  });

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

}  // namespace wpi
