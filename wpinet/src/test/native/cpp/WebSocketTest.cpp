// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/WebSocket.h"  // NOLINT(build/include_order)

#include "WebSocketTest.h"

#include <utility>
#include <vector>

#include <wpi/StringExtras.h>

#include "wpinet/HttpParser.h"

namespace wpi {

#ifdef _WIN32
const char* WebSocketTest::pipeName = "\\\\.\\pipe\\websocket-unit-test";
#else
const char* WebSocketTest::pipeName = "/tmp/websocket-unit-test";
#endif
const uint8_t WebSocketTest::testMask[4] = {0x11, 0x22, 0x33, 0x44};

void WebSocketTest::SetUpTestCase() {
#ifndef _WIN32
  unlink(pipeName);
#endif
}

std::vector<uint8_t> WebSocketTest::BuildHeader(uint8_t opcode, bool fin,
                                                bool masking, uint64_t len) {
  std::vector<uint8_t> data;
  data.push_back(opcode | (fin ? 0x80u : 0x00u));
  if (len < 126) {
    data.push_back(len | (masking ? 0x80 : 0x00u));
  } else if (len < 65536) {
    data.push_back(126u | (masking ? 0x80 : 0x00u));
    data.push_back(len >> 8);
    data.push_back(len & 0xff);
  } else {
    data.push_back(127u | (masking ? 0x80u : 0x00u));
    for (int i = 56; i >= 0; i -= 8) {
      data.push_back((len >> i) & 0xff);
    }
  }
  if (masking) {
    data.insert(data.end(), &testMask[0], &testMask[4]);
  }
  return data;
}

std::vector<uint8_t> WebSocketTest::BuildMessage(
    uint8_t opcode, bool fin, bool masking, std::span<const uint8_t> data) {
  auto finalData = BuildHeader(opcode, fin, masking, data.size());
  size_t headerSize = finalData.size();
  finalData.insert(finalData.end(), data.begin(), data.end());
  if (masking) {
    uint8_t mask[4] = {finalData[headerSize - 4], finalData[headerSize - 3],
                       finalData[headerSize - 2], finalData[headerSize - 1]};
    int n = 0;
    for (size_t i = headerSize, end = finalData.size(); i < end; ++i) {
      finalData[i] ^= mask[n++];
      if (n >= 4) {
        n = 0;
      }
    }
  }
  return finalData;
}

// If the message is masked, changes the mask to match the mask set by
// BuildHeader() by unmasking and remasking.
void WebSocketTest::AdjustMasking(std::span<uint8_t> message) {
  if (message.size() < 2) {
    return;
  }
  if ((message[1] & 0x80) == 0) {
    return;  // not masked
  }
  size_t maskPos;
  uint8_t len = message[1] & 0x7f;
  if (len == 126) {
    maskPos = 4;
  } else if (len == 127) {
    maskPos = 10;
  } else {
    maskPos = 2;
  }
  uint8_t mask[4] = {message[maskPos], message[maskPos + 1],
                     message[maskPos + 2], message[maskPos + 3]};
  message[maskPos] = testMask[0];
  message[maskPos + 1] = testMask[1];
  message[maskPos + 2] = testMask[2];
  message[maskPos + 3] = testMask[3];
  int n = 0;
  for (auto& ch : message.subspan(maskPos + 4)) {
    ch ^= mask[n] ^ testMask[n];
    if (++n >= 4) {
      n = 0;
    }
  }
}

TEST_F(WebSocketTest, CreateClientBasic) {
  int gotHost = 0;
  int gotUpgrade = 0;
  int gotConnection = 0;
  int gotKey = 0;
  int gotVersion = 0;

  HttpParser req{HttpParser::REQUEST};
  req.url.connect([](std::string_view url) { ASSERT_EQ(url, "/test"); });
  req.header.connect([&](std::string_view name, std::string_view value) {
    if (equals_lower(name, "host")) {
      ASSERT_EQ(value, pipeName);
      ++gotHost;
    } else if (equals_lower(name, "upgrade")) {
      ASSERT_EQ(value, "websocket");
      ++gotUpgrade;
    } else if (equals_lower(name, "connection")) {
      ASSERT_EQ(value, "Upgrade");
      ++gotConnection;
    } else if (equals_lower(name, "sec-websocket-key")) {
      ++gotKey;
    } else if (equals_lower(name, "sec-websocket-version")) {
      ASSERT_EQ(value, "13");
      ++gotVersion;
    } else {
      FAIL() << "unexpected header " << name;
    }
  });
  req.headersComplete.connect([&](bool) { Finish(); });

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    conn->StartRead();
    conn->data.connect([&](uv::Buffer& buf, size_t size) {
      req.Execute(std::string_view{buf.base, size});
      if (req.HasError()) {
        Finish();
      }
      ASSERT_EQ(req.GetError(), HPE_OK) << http_errno_name(req.GetError());
    });
  });
  clientPipe->Connect(pipeName, [&]() {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
  });

  loop->Run();

  if (HasFatalFailure()) {
    return;
  }
  ASSERT_EQ(gotHost, 1);
  ASSERT_EQ(gotUpgrade, 1);
  ASSERT_EQ(gotConnection, 1);
  ASSERT_EQ(gotKey, 1);
  ASSERT_EQ(gotVersion, 1);
}

TEST_F(WebSocketTest, CreateClientExtraHeaders) {
  int gotExtra1 = 0;
  int gotExtra2 = 0;
  HttpParser req{HttpParser::REQUEST};
  req.header.connect([&](std::string_view name, std::string_view value) {
    if (equals(name, "Extra1")) {
      ASSERT_EQ(value, "Data1");
      ++gotExtra1;
    } else if (equals(name, "Extra2")) {
      ASSERT_EQ(value, "Data2");
      ++gotExtra2;
    }
  });
  req.headersComplete.connect([&](bool) { Finish(); });

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    conn->StartRead();
    conn->data.connect([&](uv::Buffer& buf, size_t size) {
      req.Execute(std::string_view{buf.base, size});
      if (req.HasError()) {
        Finish();
      }
      ASSERT_EQ(req.GetError(), HPE_OK) << http_errno_name(req.GetError());
    });
  });
  clientPipe->Connect(pipeName, [&]() {
    WebSocket::ClientOptions options;
    SmallVector<std::pair<std::string_view, std::string_view>, 4> extraHeaders;
    extraHeaders.emplace_back("Extra1", "Data1");
    extraHeaders.emplace_back("Extra2", "Data2");
    options.extraHeaders = extraHeaders;
    auto ws =
        WebSocket::CreateClient(*clientPipe, "/test", pipeName, {}, options);
  });

  loop->Run();

  if (HasFatalFailure()) {
    return;
  }
  ASSERT_EQ(gotExtra1, 1);
  ASSERT_EQ(gotExtra2, 1);
}

TEST_F(WebSocketTest, CreateClientTimeout) {
  int gotClosed = 0;
  serverPipe->Listen([&]() { auto conn = serverPipe->Accept(); });
  clientPipe->Connect(pipeName, [&]() {
    WebSocket::ClientOptions options;
    options.handshakeTimeout = uv::Timer::Time{100};
    auto ws =
        WebSocket::CreateClient(*clientPipe, "/test", pipeName, {}, options);
    ws->closed.connect([&](uint16_t code, std::string_view) {
      Finish();
      ++gotClosed;
      ASSERT_EQ(code, 1006);
    });
  });

  loop->Run();

  if (HasFatalFailure()) {
    return;
  }
  ASSERT_EQ(gotClosed, 1);
}

TEST_F(WebSocketTest, CreateServerBasic) {
  int gotStatus = 0;
  int gotUpgrade = 0;
  int gotConnection = 0;
  int gotAccept = 0;
  int gotOpen = 0;

  HttpParser resp{HttpParser::RESPONSE};
  resp.status.connect([&](std::string_view status) {
    ++gotStatus;
    ASSERT_EQ(resp.GetStatusCode(), 101u) << "status: " << status;
  });
  resp.header.connect([&](std::string_view name, std::string_view value) {
    if (equals_lower(name, "upgrade")) {
      ASSERT_EQ(value, "websocket");
      ++gotUpgrade;
    } else if (equals_lower(name, "connection")) {
      ASSERT_EQ(value, "Upgrade");
      ++gotConnection;
    } else if (equals_lower(name, "sec-websocket-accept")) {
      ASSERT_EQ(value, "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
      ++gotAccept;
    } else {
      FAIL() << "unexpected header " << name;
    }
  });
  resp.headersComplete.connect([&](bool) { Finish(); });

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto ws = WebSocket::CreateServer(*conn, "dGhlIHNhbXBsZSBub25jZQ==", "13");
    ws->open.connect([&](std::string_view protocol) {
      ++gotOpen;
      ASSERT_TRUE(protocol.empty());
    });
  });
  clientPipe->Connect(pipeName, [&] {
    clientPipe->StartRead();
    clientPipe->data.connect([&](uv::Buffer& buf, size_t size) {
      resp.Execute(std::string_view{buf.base, size});
      if (resp.HasError()) {
        Finish();
      }
      ASSERT_EQ(resp.GetError(), HPE_OK) << http_errno_name(resp.GetError());
    });
  });

  loop->Run();

  if (HasFatalFailure()) {
    return;
  }
  ASSERT_EQ(gotStatus, 1);
  ASSERT_EQ(gotUpgrade, 1);
  ASSERT_EQ(gotConnection, 1);
  ASSERT_EQ(gotAccept, 1);
  ASSERT_EQ(gotOpen, 1);
}

TEST_F(WebSocketTest, CreateServerProtocol) {
  int gotProtocol = 0;
  int gotOpen = 0;

  HttpParser resp{HttpParser::RESPONSE};
  resp.header.connect([&](std::string_view name, std::string_view value) {
    if (equals_lower(name, "sec-websocket-protocol")) {
      ++gotProtocol;
      ASSERT_EQ(value, "myProtocol");
    }
  });
  resp.headersComplete.connect([&](bool) { Finish(); });

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto ws = WebSocket::CreateServer(*conn, "foo", "13", "myProtocol");
    ws->open.connect([&](std::string_view protocol) {
      ++gotOpen;
      ASSERT_EQ(protocol, "myProtocol");
    });
  });
  clientPipe->Connect(pipeName, [&] {
    clientPipe->StartRead();
    clientPipe->data.connect([&](uv::Buffer& buf, size_t size) {
      resp.Execute(std::string_view{buf.base, size});
      if (resp.HasError()) {
        Finish();
      }
      ASSERT_EQ(resp.GetError(), HPE_OK) << http_errno_name(resp.GetError());
    });
  });

  loop->Run();

  if (HasFatalFailure()) {
    return;
  }
  ASSERT_EQ(gotProtocol, 1);
  ASSERT_EQ(gotOpen, 1);
}

TEST_F(WebSocketTest, CreateServerBadVersion) {
  int gotStatus = 0;
  int gotVersion = 0;
  int gotUpgrade = 0;

  HttpParser resp{HttpParser::RESPONSE};
  resp.status.connect([&](std::string_view status) {
    ++gotStatus;
    ASSERT_EQ(resp.GetStatusCode(), 426u) << "status: " << status;
  });
  resp.header.connect([&](std::string_view name, std::string_view value) {
    if (equals_lower(name, "sec-websocket-version")) {
      ++gotVersion;
      ASSERT_EQ(value, "13");
    } else if (equals_lower(name, "upgrade")) {
      ++gotUpgrade;
      ASSERT_EQ(value, "WebSocket");
    } else {
      FAIL() << "unexpected header " << name;
    }
  });
  resp.headersComplete.connect([&](bool) { Finish(); });

  serverPipe->Listen([&] {
    auto conn = serverPipe->Accept();
    auto ws = WebSocket::CreateServer(*conn, "foo", "14");
    ws->open.connect([&](std::string_view) {
      Finish();
      FAIL();
    });
  });
  clientPipe->Connect(pipeName, [&] {
    clientPipe->StartRead();
    clientPipe->data.connect([&](uv::Buffer& buf, size_t size) {
      resp.Execute(std::string_view{buf.base, size});
      if (resp.HasError()) {
        Finish();
      }
      ASSERT_EQ(resp.GetError(), HPE_OK) << http_errno_name(resp.GetError());
    });
  });

  loop->Run();

  if (HasFatalFailure()) {
    return;
  }
  ASSERT_EQ(gotStatus, 1);
  ASSERT_EQ(gotVersion, 1);
  ASSERT_EQ(gotUpgrade, 1);
}

}  // namespace wpi
