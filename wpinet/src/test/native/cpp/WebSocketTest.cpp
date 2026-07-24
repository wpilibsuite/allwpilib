// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/WebSocket.hpp"

#ifndef _WIN32
#include <unistd.h>
#endif

#include <utility>
#include <vector>

#include "WebSocketTest.hpp"
#include "wpi/net/HttpParser.hpp"
#include "wpi/util/StringExtras.hpp"

namespace wpi::net {

#ifdef _WIN32
const char* WebSocketTest::pipeName = "\\\\.\\pipe\\websocket-unit-test";
#else
const char* WebSocketTest::pipeName = "/tmp/websocket-unit-test";
#endif
const uint8_t WebSocketTest::testMask[4] = {0x11, 0x22, 0x33, 0x44};

void WebSocketTest::UnlinkPipe() {
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

TEST_CASE_METHOD(WebSocketTest, "WebSocketTest CreateClientBasic",
                 "[websocket][client][handshake]") {
  int gotHost = 0;
  int gotUpgrade = 0;
  int gotConnection = 0;
  int gotKey = 0;
  int gotVersion = 0;

  HttpParser req{HttpParser::Type::REQUEST};
  req.url.connect([](std::string_view url) { REQUIRE(url == "/test"); });
  req.header.connect([&](std::string_view name, std::string_view value) {
    if (wpi::util::equals_lower(name, "host")) {
      REQUIRE(value == pipeName);
      ++gotHost;
    } else if (wpi::util::equals_lower(name, "upgrade")) {
      REQUIRE(value == "websocket");
      ++gotUpgrade;
    } else if (wpi::util::equals_lower(name, "connection")) {
      REQUIRE(value == "Upgrade");
      ++gotConnection;
    } else if (wpi::util::equals_lower(name, "sec-websocket-key")) {
      ++gotKey;
    } else if (wpi::util::equals_lower(name, "sec-websocket-version")) {
      REQUIRE(value == "13");
      ++gotVersion;
    } else {
      FAIL("unexpected header " << name);
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
      UNSCOPED_INFO(llhttp_errno_name(req.GetError()));
      REQUIRE(req.GetError() == HPE_PAUSED_UPGRADE);
    });
  });
  clientPipe->Connect(pipeName, [&]() {
    auto ws = WebSocket::CreateClient(*clientPipe, "/test", pipeName);
  });

  loop->Run();
  REQUIRE(gotHost == 1);
  REQUIRE(gotUpgrade == 1);
  REQUIRE(gotConnection == 1);
  REQUIRE(gotKey == 1);
  REQUIRE(gotVersion == 1);
}

TEST_CASE_METHOD(WebSocketTest, "WebSocketTest CreateClientExtraHeaders",
                 "[websocket][client][handshake]") {
  int gotExtra1 = 0;
  int gotExtra2 = 0;
  HttpParser req{HttpParser::Type::REQUEST};
  req.header.connect([&](std::string_view name, std::string_view value) {
    if (wpi::util::equals(name, "Extra1")) {
      REQUIRE(value == "Data1");
      ++gotExtra1;
    } else if (wpi::util::equals(name, "Extra2")) {
      REQUIRE(value == "Data2");
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
      UNSCOPED_INFO(llhttp_errno_name(req.GetError()));
      REQUIRE(req.GetError() == HPE_PAUSED_UPGRADE);
    });
  });
  clientPipe->Connect(pipeName, [&]() {
    WebSocket::ClientOptions options;
    wpi::util::SmallVector<std::pair<std::string_view, std::string_view>, 4>
        extraHeaders;
    extraHeaders.emplace_back("Extra1", "Data1");
    extraHeaders.emplace_back("Extra2", "Data2");
    options.extraHeaders = extraHeaders;
    auto ws =
        WebSocket::CreateClient(*clientPipe, "/test", pipeName, {}, options);
  });

  loop->Run();
  REQUIRE(gotExtra1 == 1);
  REQUIRE(gotExtra2 == 1);
}

TEST_CASE_METHOD(WebSocketTest, "WebSocketTest CreateClientTimeout",
                 "[websocket][client][handshake]") {
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
      REQUIRE(code == 1006);
    });
  });

  loop->Run();
  REQUIRE(gotClosed == 1);
}

TEST_CASE_METHOD(WebSocketTest, "WebSocketTest CreateServerBasic",
                 "[websocket][server][handshake]") {
  int gotStatus = 0;
  int gotUpgrade = 0;
  int gotConnection = 0;
  int gotAccept = 0;
  int gotOpen = 0;

  HttpParser resp{HttpParser::Type::RESPONSE};
  resp.status.connect([&](std::string_view status) {
    ++gotStatus;
    UNSCOPED_INFO("status: " << status);
    REQUIRE(resp.GetStatusCode() == 101u);
  });
  resp.header.connect([&](std::string_view name, std::string_view value) {
    if (wpi::util::equals_lower(name, "upgrade")) {
      REQUIRE(value == "websocket");
      ++gotUpgrade;
    } else if (wpi::util::equals_lower(name, "connection")) {
      REQUIRE(value == "Upgrade");
      ++gotConnection;
    } else if (wpi::util::equals_lower(name, "sec-websocket-accept")) {
      REQUIRE(value == "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
      ++gotAccept;
    } else {
      FAIL("unexpected header " << name);
    }
  });
  resp.headersComplete.connect([&](bool) { Finish(); });

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto ws = WebSocket::CreateServer(*conn, "dGhlIHNhbXBsZSBub25jZQ==", "13");
    ws->open.connect([&](std::string_view protocol) {
      ++gotOpen;
      REQUIRE(protocol.empty());
    });
  });
  clientPipe->Connect(pipeName, [&] {
    clientPipe->StartRead();
    clientPipe->data.connect([&](uv::Buffer& buf, size_t size) {
      resp.Execute(std::string_view{buf.base, size});
      if (resp.HasError()) {
        Finish();
      }
      UNSCOPED_INFO(llhttp_errno_name(resp.GetError()));
      REQUIRE(resp.GetError() == HPE_PAUSED_UPGRADE);
    });
  });

  loop->Run();
  REQUIRE(gotStatus == 1);
  REQUIRE(gotUpgrade == 1);
  REQUIRE(gotConnection == 1);
  REQUIRE(gotAccept == 1);
  REQUIRE(gotOpen == 1);
}

TEST_CASE_METHOD(WebSocketTest, "WebSocketTest CreateServerProtocol",
                 "[websocket][server][handshake][protocol]") {
  int gotProtocol = 0;
  int gotOpen = 0;

  HttpParser resp{HttpParser::Type::RESPONSE};
  resp.header.connect([&](std::string_view name, std::string_view value) {
    if (wpi::util::equals_lower(name, "sec-websocket-protocol")) {
      ++gotProtocol;
      REQUIRE(value == "myProtocol");
    }
  });
  resp.headersComplete.connect([&](bool) { Finish(); });

  serverPipe->Listen([&]() {
    auto conn = serverPipe->Accept();
    auto ws = WebSocket::CreateServer(*conn, "foo", "13", "myProtocol");
    ws->open.connect([&](std::string_view protocol) {
      ++gotOpen;
      REQUIRE(protocol == "myProtocol");
    });
  });
  clientPipe->Connect(pipeName, [&] {
    clientPipe->StartRead();
    clientPipe->data.connect([&](uv::Buffer& buf, size_t size) {
      resp.Execute(std::string_view{buf.base, size});
      if (resp.HasError()) {
        Finish();
      }
      UNSCOPED_INFO(llhttp_errno_name(resp.GetError()));
      REQUIRE(resp.GetError() == HPE_PAUSED_UPGRADE);
    });
  });

  loop->Run();
  REQUIRE(gotProtocol == 1);
  REQUIRE(gotOpen == 1);
}

TEST_CASE_METHOD(WebSocketTest, "WebSocketTest CreateServerBadVersion",
                 "[websocket][server][handshake][protocol]") {
  int gotStatus = 0;
  int gotVersion = 0;
  int gotUpgrade = 0;

  HttpParser resp{HttpParser::Type::RESPONSE};
  resp.status.connect([&](std::string_view status) {
    ++gotStatus;
    UNSCOPED_INFO("status: " << status);
    REQUIRE(resp.GetStatusCode() == 426u);
  });
  resp.header.connect([&](std::string_view name, std::string_view value) {
    if (wpi::util::equals_lower(name, "sec-websocket-version")) {
      ++gotVersion;
      REQUIRE(value == "13");
    } else if (wpi::util::equals_lower(name, "upgrade")) {
      ++gotUpgrade;
      REQUIRE(value == "WebSocket");
    } else {
      FAIL("unexpected header " << name);
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
      UNSCOPED_INFO(llhttp_errno_name(resp.GetError()));
      REQUIRE(resp.GetError() == HPE_OK);
    });
  });

  loop->Run();
  REQUIRE(gotStatus == 1);
  REQUIRE(gotVersion == 1);
  REQUIRE(gotUpgrade == 1);
}

}  // namespace wpi::net
