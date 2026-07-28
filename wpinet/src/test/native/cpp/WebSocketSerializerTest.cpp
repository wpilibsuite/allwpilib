// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WebSocketSerializer.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <span>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "WebSocketTest.hpp"
#include "wpi/net/uv/Buffer.hpp"

namespace wpi::net::uv {
inline bool operator==(const Buffer& lhs, const Buffer& rhs) {
  return lhs.len == rhs.len &&
         std::equal(lhs.base, lhs.base + lhs.len, rhs.base);
}
}  // namespace wpi::net::uv

namespace wpi::net {
inline bool operator==(const WebSocket::Frame& lhs,
                       const WebSocket::Frame& rhs) {
  return lhs.opcode == rhs.opcode && lhs.data.size() == rhs.data.size() &&
         std::equal(lhs.data.begin(), lhs.data.end(), rhs.data.begin());
}
}  // namespace wpi::net

namespace wpi::net::detail {

template <typename T>
bool SpanEquals(std::span<const T> lhs, std::span<const T> rhs) {
  return lhs.size() == rhs.size() &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T>
bool SpanEquals(std::span<const T> lhs, const std::vector<T>& rhs) {
  return SpanEquals(lhs, std::span<const T>{rhs});
}

template <typename T, size_t N>
bool SpanEquals(std::span<const T> lhs, const std::array<T, N>& rhs) {
  return SpanEquals(lhs, std::span<const T>{rhs});
}

template <typename T, size_t N>
bool SpanEquals(std::span<const T> lhs, const T (&rhs)[N]) {
  return SpanEquals(lhs, std::span<const T>{rhs});
}

class MockWebSocketWriteReq
    : public std::enable_shared_from_this<MockWebSocketWriteReq>,
      public detail::WebSocketWriteReqBase {
 public:
  explicit MockWebSocketWriteReq(
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {}
};

class MockStream {
 public:
  ~MockStream() {
    CHECK(m_tryWriteCalls == m_expectedTryWriteCalls);
    CHECK(m_writeCalls == m_expectedWrites.size());
  }

  void ExpectTryWrite(int result) {
    m_tryWriteResult = result;
    m_expectedTryWriteCalls = 1;
    m_checkTryWriteBufs = false;
  }

  template <typename Range>
  void ExpectTryWrite(const Range& bufs, int result) {
    ExpectTryWrite(result);
    m_expectedTryWriteBufs.assign(bufs.begin(), bufs.end());
    m_checkTryWriteBufs = true;
  }

  template <typename Range>
  void ExpectWrite(const Range& bufs) {
    m_expectedWrites.emplace_back(bufs.begin(), bufs.end());
  }

  int TryWrite(std::span<const uv::Buffer> bufs) {
    REQUIRE(m_tryWriteCalls < m_expectedTryWriteCalls);
    if (m_checkTryWriteBufs) {
      REQUIRE(SpanEquals(bufs, m_expectedTryWriteBufs));
    }
    ++m_tryWriteCalls;
    return m_tryWriteResult;
  }

  void Write(std::span<const uv::Buffer> bufs,
             const std::shared_ptr<MockWebSocketWriteReq>& req) {
    REQUIRE(m_writeCalls < m_expectedWrites.size());
    REQUIRE(SpanEquals(bufs, m_expectedWrites[m_writeCalls]));
    ++m_writeCalls;
  }

  wpi::util::Logger* GetLogger() const { return nullptr; }

 private:
  int m_tryWriteResult = 0;
  size_t m_expectedTryWriteCalls = 0;
  size_t m_tryWriteCalls = 0;
  bool m_checkTryWriteBufs = false;
  std::vector<uv::Buffer> m_expectedTryWriteBufs;
  std::vector<std::vector<uv::Buffer>> m_expectedWrites;
  size_t m_writeCalls = 0;
};

class WebSocketWriteReqTest {
 public:
  WebSocketWriteReqTest() {
    req->m_frames.m_bufs.emplace_back(m_buf0);
    req->m_frames.m_bufs.emplace_back(m_buf1);
    req->m_frames.m_bufs.emplace_back(m_buf2);
    req->m_continueFrameOffs.emplace_back(5);  // frame 0: first 2 buffers
    req->m_continueFrameOffs.emplace_back(9);  // frame 1: last buffer
  }

  std::shared_ptr<MockWebSocketWriteReq> req =
      std::make_shared<MockWebSocketWriteReq>([](auto, auto) {});
  MockStream stream;
  static const uint8_t m_buf0[3];
  static const uint8_t m_buf1[2];
  static const uint8_t m_buf2[4];
};

const uint8_t WebSocketWriteReqTest::m_buf0[3] = {1, 2, 3};
const uint8_t WebSocketWriteReqTest::m_buf1[2] = {4, 5};
const uint8_t WebSocketWriteReqTest::m_buf2[4] = {6, 7, 8, 9};

TEST_CASE_METHOD(WebSocketWriteReqTest, "WebSocketWriteReqTest ContinueDone",
                 "[websocket][serializer][write-request]") {
  req->m_continueBufPos = 3;
  REQUIRE(req->Continue(stream, req) == 0);
}

TEST_CASE_METHOD(WebSocketWriteReqTest,
                 "WebSocketWriteReqTest ContinueTryWriteComplete",
                 "[websocket][serializer][write-request]") {
  stream.ExpectTryWrite(req->m_frames.m_bufs, 9);
  REQUIRE(req->Continue(stream, req) == 0);
}

TEST_CASE_METHOD(WebSocketWriteReqTest,
                 "WebSocketWriteReqTest ContinueTryWriteNoProgress",
                 "[websocket][serializer][write-request]") {
  // if TryWrite returns 0
  stream.ExpectTryWrite(0);
  // Write should get called for all of next frame - make forward progress
  uv::Buffer remBufs[2] = {uv::Buffer{m_buf0}, uv::Buffer{m_buf1}};
  stream.ExpectWrite(std::span<const uv::Buffer>(remBufs));
  REQUIRE(req->Continue(stream, req) == 1);
}

TEST_CASE_METHOD(WebSocketWriteReqTest,
                 "WebSocketWriteReqTest ContinueTryWriteError",
                 "[websocket][serializer][write-request]") {
  // if TryWrite returns -1, the error is passed along
  stream.ExpectTryWrite(-1);
  REQUIRE(req->Continue(stream, req) == -1);
}

TEST_CASE_METHOD(WebSocketWriteReqTest,
                 "WebSocketWriteReqTest ContinueTryWritePartialMidFrameMidBuf1",
                 "[websocket][serializer][write-request]") {
  // stop partway through buf 0
  stream.ExpectTryWrite(2);
  // Write should get called for remainder of buf 0 and all of buf 1
  uv::Buffer remBufs[2] = {uv::Buffer{&m_buf0[2], 1}, uv::Buffer{m_buf1}};
  stream.ExpectWrite(std::span<const uv::Buffer>(remBufs));
  REQUIRE(req->Continue(stream, req) == 1);
}

TEST_CASE_METHOD(
    WebSocketWriteReqTest,
    "WebSocketWriteReqTest ContinueTryWritePartialMidFrameBufBoundary",
    "[websocket][serializer][write-request]") {
  // stop at end of buf 0
  stream.ExpectTryWrite(3);
  // Write should get called for all of buf 1
  uv::Buffer remBufs[1] = {uv::Buffer{m_buf1}};
  stream.ExpectWrite(std::span<const uv::Buffer>(remBufs));
  REQUIRE(req->Continue(stream, req) == 1);
}

TEST_CASE_METHOD(WebSocketWriteReqTest,
                 "WebSocketWriteReqTest ContinueTryWritePartialMidFrameMidBuf2",
                 "[websocket][serializer][write-request]") {
  // stop partway through buf 1
  stream.ExpectTryWrite(4);
  // Write should get called for remainder of buf 1
  uv::Buffer remBufs[1] = {uv::Buffer{&m_buf1[1], 1}};
  stream.ExpectWrite(std::span<const uv::Buffer>(remBufs));
  REQUIRE(req->Continue(stream, req) == 1);
}

TEST_CASE_METHOD(WebSocketWriteReqTest,
                 "WebSocketWriteReqTest ContinueTryWritePartialFrameBoundary",
                 "[websocket][serializer][write-request]") {
  // stop at end of buf 1
  stream.ExpectTryWrite(5);
  // Write should get called for all of next frame
  uv::Buffer remBufs[1] = {uv::Buffer{m_buf2}};
  stream.ExpectWrite(std::span<const uv::Buffer>(remBufs));
  REQUIRE(req->Continue(stream, req) == 1);
}

TEST_CASE_METHOD(WebSocketWriteReqTest,
                 "WebSocketWriteReqTest ContinueTryWritePartialMidFrameMidBuf3",
                 "[websocket][serializer][write-request]") {
  // stop partway through buf 2
  stream.ExpectTryWrite(6);
  // Write should get called for remainder of buf 2
  uv::Buffer remBufs[1] = {uv::Buffer{&m_buf2[1], 3}};
  stream.ExpectWrite(std::span<const uv::Buffer>(remBufs));
  REQUIRE(req->Continue(stream, req) == 1);
}

class WebSocketTrySendTest {
 public:
  MockStream stream;
  std::shared_ptr<MockWebSocketWriteReq> req;
  static const std::array<uint8_t, 3> m_buf0data;
  static const std::array<uint8_t, 2> m_buf1data;
  static const std::array<uint8_t, 4> m_buf2data;
  static const std::array<uint8_t, 4> m_buf3data;
  static const std::array<uv::Buffer, 4> m_bufs;
  static const std::array<uint8_t, 5> m_frame0data;
  static const std::array<uv::Buffer, 2> m_frame0bufs;
  static const std::array<uv::Buffer, 1> m_frame1bufs;
  static const std::array<uv::Buffer, 1> m_frame2bufs;
  static const std::array<WebSocket::Frame, 3> m_frames;
  static const std::array<std::vector<uint8_t>, 3> m_serialized;
  static const std::array<uv::Buffer, 3> m_frameHeaders;

  int makeReqCalled = 0;
  int callbackCalled = 0;
  void CheckTrySendFrames(std::span<const uv::Buffer> expectCbBufs,
                          std::span<const WebSocket::Frame> expectRet,
                          int expectErr = 0);
};

const std::array<uint8_t, 3> WebSocketTrySendTest::m_buf0data{1, 2, 3};
const std::array<uint8_t, 2> WebSocketTrySendTest::m_buf1data{4, 5};
const std::array<uint8_t, 4> WebSocketTrySendTest::m_buf2data{6, 7, 8, 9};
const std::array<uint8_t, 4> WebSocketTrySendTest::m_buf3data{10, 11, 12, 13};
const std::array<uv::Buffer, 4> WebSocketTrySendTest::m_bufs{
    uv::Buffer{m_buf0data}, uv::Buffer{m_buf1data}, uv::Buffer{m_buf2data},
    uv::Buffer{m_buf3data}};
const std::array<uint8_t, 5> WebSocketTrySendTest::m_frame0data{1, 2, 3, 4, 5};
const std::array<uv::Buffer, 2> WebSocketTrySendTest::m_frame0bufs{m_bufs[0],
                                                                   m_bufs[1]};
const std::array<uv::Buffer, 1> WebSocketTrySendTest::m_frame1bufs{m_bufs[2]};
const std::array<uv::Buffer, 1> WebSocketTrySendTest::m_frame2bufs{m_bufs[3]};
const std::array<WebSocket::Frame, 3> WebSocketTrySendTest::m_frames{
    WebSocket::Frame{WebSocket::Frame::BINARY_FRAGMENT, m_frame0bufs},
    WebSocket::Frame{WebSocket::Frame::BINARY, m_frame1bufs},
    WebSocket::Frame{WebSocket::Frame::TEXT, m_frame2bufs},
};
const std::array<std::vector<uint8_t>, 3> WebSocketTrySendTest::m_serialized{
    WebSocketTest::BuildMessage(m_frames[0].opcode, false, false, m_frame0data),
    WebSocketTest::BuildMessage(m_frames[1].opcode, true, false, m_buf2data),
    WebSocketTest::BuildMessage(m_frames[2].opcode, true, false, m_buf3data),
};
const std::array<uv::Buffer, 3> WebSocketTrySendTest::m_frameHeaders{
    uv::Buffer{m_serialized[0].data(),
               m_serialized[0].size() - m_frame0data.size()},
    uv::Buffer{m_serialized[1].data(),
               m_serialized[1].size() - m_buf2data.size()},
    uv::Buffer{m_serialized[2].data(),
               m_serialized[2].size() - m_buf3data.size()},
};

void WebSocketTrySendTest::CheckTrySendFrames(
    std::span<const uv::Buffer> expectCbBufs,
    std::span<const WebSocket::Frame> expectRet, int expectErr) {
  auto remaining = TrySendFrames(
      true, stream, m_frames,
      [&](std::function<void(std::span<uv::Buffer>, uv::Error)>&& cb) {
        ++makeReqCalled;
        req = std::make_shared<MockWebSocketWriteReq>(std::move(cb));
        return req;
      },
      [&](auto bufs, auto err) {
        ++callbackCalled;
        REQUIRE(SpanEquals(std::span<const uv::Buffer>{bufs}, expectCbBufs));
        REQUIRE(err.code() == expectErr);
      });
  REQUIRE(SpanEquals(std::span<const WebSocket::Frame>{remaining}, expectRet));
}

TEST_CASE_METHOD(WebSocketTrySendTest, "WebSocketTrySendTest ServerComplete",
                 "[websocket][serializer][send]") {
  // if trywrite sends everything
  stream.ExpectTryWrite(m_serialized[0].size() + m_serialized[1].size() +
                        m_serialized[2].size());
  // return nothing, and call callback immediately
  CheckTrySendFrames(m_bufs, {});
  REQUIRE(makeReqCalled == 0);
  REQUIRE(callbackCalled == 1);
}

TEST_CASE_METHOD(WebSocketTrySendTest, "WebSocketTrySendTest ServerNoProgress",
                 "[websocket][serializer][send]") {
  // if trywrite sends nothing
  stream.ExpectTryWrite(0);
  // we should get all the frames back (the callback may be called with an empty
  // set of buffers)
  CheckTrySendFrames({}, m_frames);
  REQUIRE(makeReqCalled == 0);
  REQUIRE((callbackCalled == 0 || callbackCalled == 1));
}

TEST_CASE_METHOD(WebSocketTrySendTest, "WebSocketTrySendTest ServerError",
                 "[websocket][serializer][send]") {
  // if TryWrite returns -1, the error is passed along
  stream.ExpectTryWrite(-1);
  CheckTrySendFrames(m_bufs, m_frames, -1);
  REQUIRE(makeReqCalled == 0);
  REQUIRE(callbackCalled == 1);
}

TEST_CASE_METHOD(WebSocketTrySendTest,
                 "WebSocketTrySendTest ServerPartialMidFrameMidBuf0",
                 "[websocket][serializer][send]") {
  // stop partway through buf 0 (first buf of frame 0)
  stream.ExpectTryWrite(m_frameHeaders[0].len + 2);
  // Write should get called for remainder of buf 0 and all of buf 1
  // buf 2 should get put into continuation because frame 0 is a fragment
  // return will be frame 2 only
  std::array<uv::Buffer, 2> remBufs{std::span{m_buf0data}.subspan(2),
                                    m_bufs[1]};
  std::array<uv::Buffer, 2> contBufs{m_frameHeaders[1], m_bufs[2]};
  std::array<int, 1> contFrameOffs{static_cast<int>(m_serialized[1].size())};
  stream.ExpectWrite(remBufs);
  CheckTrySendFrames({}, std::span{m_frames}.subspan(2));
  REQUIRE(makeReqCalled == 1);
  REQUIRE(
      SpanEquals(std::span<const uv::Buffer>{req->m_frames.m_bufs}, contBufs));
  REQUIRE(req->m_continueBufPos == 0u);
  REQUIRE(req->m_continueFramePos == 0u);
  REQUIRE(SpanEquals(std::span<const int>{req->m_continueFrameOffs},
                     contFrameOffs));
  REQUIRE(callbackCalled == 0);
}

TEST_CASE_METHOD(WebSocketTrySendTest,
                 "WebSocketTrySendTest ServerPartialMidFrameBufBoundary",
                 "[websocket][serializer][send]") {
  // stop at end of buf 0 (first buf of frame 0)
  stream.ExpectTryWrite(m_frameHeaders[0].len + 3);
  // Write should get called for all of buf 1
  // buf 2 should get put into continuation because frame 0 is a fragment
  // return will be frame 2 only
  std::array<uv::Buffer, 1> remBufs{m_bufs[1]};
  std::array<uv::Buffer, 2> contBufs{m_frameHeaders[1], m_bufs[2]};
  stream.ExpectWrite(remBufs);
  CheckTrySendFrames({}, std::span{m_frames}.subspan(2));
  REQUIRE(makeReqCalled == 1);
  REQUIRE(
      SpanEquals(std::span<const uv::Buffer>{req->m_frames.m_bufs}, contBufs));
  REQUIRE(callbackCalled == 0);
}

TEST_CASE_METHOD(WebSocketTrySendTest,
                 "WebSocketTrySendTest ServerPartialMidFrameMidBuf1",
                 "[websocket][serializer][send]") {
  // stop partway through buf 1 (second buf of frame 0)
  stream.ExpectTryWrite(m_frameHeaders[0].len + 4);
  // Write should get called for remainder of buf 1
  // buf 2 should get put into continuation because frame 0 is a fragment
  // return will be frame 2 only
  std::array<uv::Buffer, 1> remBufs{std::span{m_buf1data}.subspan(1)};
  std::array<uv::Buffer, 2> contBufs{m_frameHeaders[1], m_bufs[2]};
  stream.ExpectWrite(remBufs);
  CheckTrySendFrames({}, std::span{m_frames}.subspan(2));
  REQUIRE(makeReqCalled == 1);
  REQUIRE(
      SpanEquals(std::span<const uv::Buffer>{req->m_frames.m_bufs}, contBufs));
  REQUIRE(callbackCalled == 0);
}

TEST_CASE_METHOD(WebSocketTrySendTest,
                 "WebSocketTrySendTest ServerPartialFrameBoundary",
                 "[websocket][serializer][send]") {
  // stop at end of buf 1 (end of frame 0)
  stream.ExpectTryWrite(m_frameHeaders[0].len + m_frameHeaders[1].len + 5);
  // Write should get called for all of buf 2 because frame 0 is a fragment
  // no continuation
  // return will be frame 2 only
  std::array<uv::Buffer, 1> remBufs{m_bufs[2]};
  stream.ExpectWrite(remBufs);
  CheckTrySendFrames({}, std::span{m_frames}.subspan(2));
  REQUIRE(makeReqCalled == 1);
  REQUIRE(req->m_frames.m_bufs.empty());
  REQUIRE(callbackCalled == 0);
}

TEST_CASE_METHOD(WebSocketTrySendTest,
                 "WebSocketTrySendTest ServerPartialMidFrameMidBuf2",
                 "[websocket][serializer][send]") {
  // stop partway through buf 2 (frame 1 buf)
  stream.ExpectTryWrite(m_frameHeaders[0].len + m_frameHeaders[1].len + 6);
  // Write should get called for remainder of buf 2; no continuation
  // return will be frame 2 only
  std::array<uv::Buffer, 1> remBufs{std::span{m_buf2data}.subspan(1)};
  stream.ExpectWrite(remBufs);
  CheckTrySendFrames({}, std::span{m_frames}.subspan(2));
  REQUIRE(makeReqCalled == 1);
  REQUIRE(req->m_frames.m_bufs.empty());
  REQUIRE(callbackCalled == 0);
}

TEST_CASE_METHOD(WebSocketTrySendTest,
                 "WebSocketTrySendTest ServerFrameBoundary",
                 "[websocket][serializer][send]") {
  // stop at end of buf 2 (end of frame 1)
  stream.ExpectTryWrite(m_frameHeaders[0].len + m_frameHeaders[1].len + 9);
  // call callback immediately for bufs 0-2 and return frame 2
  CheckTrySendFrames(std::span{m_bufs}.subspan(0, 3),
                     std::span{m_frames}.subspan(2));
  REQUIRE(makeReqCalled == 0);
  REQUIRE(callbackCalled == 1);
}

TEST_CASE_METHOD(WebSocketTrySendTest,
                 "WebSocketTrySendTest ServerPartialLastFrame",
                 "[websocket][serializer][send]") {
  // stop partway through buf 3
  stream.ExpectTryWrite(m_frameHeaders[0].len + m_frameHeaders[1].len +
                        m_frameHeaders[2].len + 10);
  // Write called for remainder of buf 3; no continuation
  std::array<uv::Buffer, 1> remBufs{std::span{m_buf3data}.subspan(1)};
  stream.ExpectWrite(remBufs);
  CheckTrySendFrames({}, {});
  REQUIRE(makeReqCalled == 1);
  REQUIRE(req->m_frames.m_bufs.empty());
  REQUIRE(callbackCalled == 0);
}

TEST_CASE_METHOD(WebSocketTrySendTest, "WebSocketTrySendTest Big",
                 "[websocket][serializer][send]") {
  std::vector<uv::Buffer> bufs;
  for (int i = 0; i < 100000;) {
    i += 1430;
    bufs.emplace_back(
        uv::Buffer::Allocate(i < 100000 ? 1430 : (100000 - (i - 1430))));
  }
  WebSocket::Frame frame{WebSocket::OP_BINARY | WebSocket::FLAG_FIN, bufs};
  stream.ExpectTryWrite(7681);

  // Write called for remainder of buffers
  std::vector<uv::Buffer> remBufs;
  remBufs.emplace_back(bufs[5]);
  remBufs.back().base += 521;
  remBufs.back().len -= 521;
  for (size_t i = 6; i < bufs.size(); ++i) {
    remBufs.emplace_back(bufs[i]);
  }
  stream.ExpectWrite(remBufs);

  REQUIRE(TrySendFrames(
              true, stream, {{frame}},
              [&](std::function<void(std::span<uv::Buffer>, uv::Error)>&& cb) {
                ++makeReqCalled;
                req = std::make_shared<MockWebSocketWriteReq>(std::move(cb));
                return req;
              },
              [&](auto bufs, auto err) { ++callbackCalled; })
              .empty());
  for (auto& buf : bufs) {
    buf.Deallocate();
  }
  REQUIRE(makeReqCalled == 1);
  REQUIRE(req->m_frames.m_bufs.empty());
  REQUIRE(callbackCalled == 0);
}

}  // namespace wpi::net::detail
