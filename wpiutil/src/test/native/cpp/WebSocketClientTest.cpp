/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/WebSocket.h"  // NOLINT(build/include_order)

#include "WebSocketTest.h"
#include "wpi/Base64.h"
#include "wpi/HttpParser.h"
#include "wpi/SmallString.h"
#include "wpi/raw_uv_ostream.h"
#include "wpi/sha1.h"

namespace wpi {

class WebSocketClientTest : public WebSocketTest {
 public:
  WebSocketClientTest() {
    // Bare bones server
    req.header.connect([this](StringRef name, StringRef value) {
      // save key (required for valid response)
      if (name.equals_lower("sec-websocket-key")) clientKey = value;
    });
    req.headersComplete.connect([this](bool) {
      // send response
      SmallVector<uv::Buffer, 4> bufs;
      raw_uv_ostream os{bufs, 4096};
      os << "HTTP/1.1 101 Switching Protocols\r\n";
      os << "Upgrade: websocket\r\n";
      os << "Connection: Upgrade\r\n";

      // accept hash
      SHA1 hash;
      hash.Update(clientKey);
      hash.Update("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
      if (mockBadAccept) hash.Update("1");
      SmallString<64> hashBuf;
      SmallString<64> acceptBuf;
      os << "Sec-WebSocket-Accept: "
         << Base64Encode(hash.RawFinal(hashBuf), acceptBuf) << "\r\n";

      if (!mockProtocol.empty())
        os << "Sec-WebSocket-Protocol: " << mockProtocol << "\r\n";

      os << "\r\n";

      conn->Write(bufs, [](auto bufs, uv::Error) {
        for (auto& buf : bufs) buf.Deallocate();
      });

      serverHeadersDone = true;
      if (connected) connected();
    });

    serverPipe->Listen([this] {
      conn = serverPipe->Accept();
      conn->StartRead();
      conn->data.connect([this](uv::Buffer& buf, size_t size) {
        StringRef data{buf.base, size};
        if (!serverHeadersDone) {
          data = req.Execute(data);
          if (req.HasError()) Finish();
          ASSERT_EQ(req.GetError(), HPE_OK) << http_errno_name(req.GetError());
          if (data.empty()) return;
        }
        wireData.insert(wireData.end(), data.bytes_begin(), data.bytes_end());
      });
      conn->end.connect([this] { Finish(); });
    });
  }

  bool mockBadAccept = false;
  std::vector<uint8_t> wireData;
  std::shared_ptr<uv::Pipe> conn;
  HttpParser req{HttpParser::kRequest};
  SmallString<64> clientKey;
  std::string mockProtocol;
  bool serverHeadersDone = false;
  std::function<void()> connected;
};

TEST_F(WebSocketClientTest, Open) {
  int gotOpen = 0;

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, StringRef reason) {
      Finish();
      if (code != 1005 && code != 1006)
        FAIL() << "Code: " << code << " Reason: " << reason;
    });
    ws->open.connect([&](StringRef protocol) {
      ++gotOpen;
      Finish();
      ASSERT_TRUE(protocol.empty());
    });
  });

  loop->Run();

  if (HasFatalFailure()) return;
  ASSERT_EQ(gotOpen, 1);
}

TEST_F(WebSocketClientTest, BadAccept) {
  int gotClosed = 0;

  mockBadAccept = true;

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, StringRef msg) {
      Finish();
      ++gotClosed;
      ASSERT_EQ(code, 1002) << "Message: " << msg;
    });
    ws->open.connect([&](StringRef protocol) {
      Finish();
      FAIL() << "Got open";
    });
  });

  loop->Run();

  if (HasFatalFailure()) return;
  ASSERT_EQ(gotClosed, 1);
}

TEST_F(WebSocketClientTest, ProtocolGood) {
  int gotOpen = 0;

  mockProtocol = "myProtocol";

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(
        *clientPipe, "/test", pipeName,
        ArrayRef<StringRef>{"myProtocol", "myProtocol2"});
    ws->closed.connect([&](uint16_t code, StringRef msg) {
      Finish();
      if (code != 1005 && code != 1006)
        FAIL() << "Code: " << code << "Message: " << msg;
    });
    ws->open.connect([&](StringRef protocol) {
      ++gotOpen;
      Finish();
      ASSERT_EQ(protocol, "myProtocol");
    });
  });

  loop->Run();

  if (HasFatalFailure()) return;
  ASSERT_EQ(gotOpen, 1);
}

TEST_F(WebSocketClientTest, ProtocolRespNotReq) {
  int gotClosed = 0;

  mockProtocol = "myProtocol";

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
    ws->closed.connect([&](uint16_t code, StringRef msg) {
      Finish();
      ++gotClosed;
      ASSERT_EQ(code, 1003) << "Message: " << msg;
    });
    ws->open.connect([&](StringRef protocol) {
      Finish();
      FAIL() << "Got open";
    });
  });

  loop->Run();

  if (HasFatalFailure()) return;
  ASSERT_EQ(gotClosed, 1);
}

TEST_F(WebSocketClientTest, ProtocolReqNotResp) {
  int gotClosed = 0;

  clientPipe->Connect(pipeName, [&] {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName,
                                      StringRef{"myProtocol"});
    ws->closed.connect([&](uint16_t code, StringRef msg) {
      Finish();
      ++gotClosed;
      ASSERT_EQ(code, 1002) << "Message: " << msg;
    });
    ws->open.connect([&](StringRef protocol) {
      Finish();
      FAIL() << "Got open";
    });
  });

  loop->Run();

  if (HasFatalFailure()) return;
  ASSERT_EQ(gotClosed, 1);
}

//
// Send and receive data.  Most of these cases are tested in
// WebSocketServerTest, so only spot check differences like masking.
//

class WebSocketClientDataTest : public WebSocketClientTest,
                                public ::testing::WithParamInterface<size_t> {
 public:
  WebSocketClientDataTest() {
    clientPipe->Connect(pipeName, [&] {
      ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
      if (setupWebSocket) setupWebSocket();
    });
  }

  std::function<void()> setupWebSocket;
  std::shared_ptr<WebSocket> ws;
};

INSTANTIATE_TEST_CASE_P(WebSocketClientDataTests, WebSocketClientDataTest,
                        ::testing::Values(0, 1, 125, 126, 65535, 65536), );

TEST_P(WebSocketClientDataTest, SendBinary) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), 0x03u);
  setupWebSocket = [&] {
    ws->open.connect([&](StringRef) {
      ws->SendBinary(uv::Buffer(data), [&](auto bufs, uv::Error) {
        ++gotCallback;
        ws->Terminate();
        ASSERT_FALSE(bufs.empty());
        ASSERT_EQ(bufs[0].base, reinterpret_cast<const char*>(data.data()));
      });
    });
  };

  loop->Run();

  auto expectData = BuildMessage(0x02, true, true, data);
  AdjustMasking(wireData);
  ASSERT_EQ(wireData, expectData);
  ASSERT_EQ(gotCallback, 1);
}

TEST_P(WebSocketClientDataTest, ReceiveBinary) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), 0x03u);
  setupWebSocket = [&] {
    ws->binary.connect([&](ArrayRef<uint8_t> inData, bool fin) {
      ++gotCallback;
      ws->Terminate();
      ASSERT_TRUE(fin);
      std::vector<uint8_t> recvData{inData.begin(), inData.end()};
      ASSERT_EQ(data, recvData);
    });
  };
  auto message = BuildMessage(0x02, true, false, data);
  connected = [&] {
    conn->Write(uv::Buffer(message), [&](auto bufs, uv::Error) {});
  };

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

//
// The client must close the connection if a masked frame is received.
//

TEST_P(WebSocketClientDataTest, ReceiveMasked) {
  int gotCallback = 0;
  std::vector<uint8_t> data(GetParam(), ' ');
  setupWebSocket = [&] {
    ws->text.connect([&](StringRef, bool) {
      ws->Terminate();
      FAIL() << "Should not have gotten masked message";
    });
    ws->closed.connect([&](uint16_t code, StringRef reason) {
      ++gotCallback;
      ASSERT_EQ(code, 1002) << "reason: " << reason;
    });
  };
  auto message = BuildMessage(0x01, true, true, data);
  connected = [&] {
    conn->Write(uv::Buffer(message), [&](auto bufs, uv::Error) {});
  };

  loop->Run();

  ASSERT_EQ(gotCallback, 1);
}

}  // namespace wpi
