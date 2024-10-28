// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WebSocketSerializer.h"  // NOLINT(build/include_order)

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <ostream>
#include <span>
#include <utility>
#include <vector>

#include <gmock/gmock.h>
#include <wpi/SpanMatcher.h>

#include "WebSocketTest.h"
#include "wpinet/uv/Buffer.h"

using ::testing::_;
using ::testing::AnyOf;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::Pointee;
using ::testing::Return;

namespace wpi::uv {
inline bool operator==(const Buffer& lhs, const Buffer& rhs) {
  return lhs.len == rhs.len &&
         std::equal(lhs.base, lhs.base + lhs.len, rhs.base);
}
inline void PrintTo(const Buffer& buf, ::std::ostream* os) {
  ::wpi::PrintTo(buf.bytes(), os);
}
}  // namespace wpi::uv

namespace wpi {
inline bool operator==(const WebSocket::Frame& lhs,
                       const WebSocket::Frame& rhs) {
  return lhs.opcode == rhs.opcode &&
         std::equal(lhs.data.begin(), lhs.data.end(), rhs.data.begin());
}
inline void PrintTo(const WebSocket::Frame& frame, ::std::ostream* os) {
  *os << frame.opcode << ": ";
  ::wpi::PrintTo(frame.data, os);
}
}  // namespace wpi

namespace wpi::detail {

class MockWebSocketWriteReq
    : public std::enable_shared_from_this<MockWebSocketWriteReq>,
      public detail::WebSocketWriteReqBase {
 public:
  explicit MockWebSocketWriteReq(
      std::function<void(std::span<uv::Buffer>, uv::Error)> callback) {}
};

class MockStream {
 public:
  MOCK_METHOD(int, TryWrite, (std::span<const uv::Buffer>));
  void Write(std::span<const uv::Buffer> bufs,
             const std::shared_ptr<MockWebSocketWriteReq>& req) {
    // std::cout << "Write(";
    // PrintTo(bufs, &std::cout);
    // std::cout << ")\n";
    DoWrite(bufs, req);
  }
  MOCK_METHOD(void, DoWrite,
              (std::span<const uv::Buffer> bufs,
               const std::shared_ptr<MockWebSocketWriteReq>& req));

  Logger* GetLogger() const { return nullptr; }
};

class WebSocketWriteReqTest : public ::testing::Test {
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
  ::testing::StrictMock<MockStream> stream;
  static const uint8_t m_buf0[3];
  static const uint8_t m_buf1[2];
  static const uint8_t m_buf2[4];
};

const uint8_t WebSocketWriteReqTest::m_buf0[3] = {1, 2, 3};
const uint8_t WebSocketWriteReqTest::m_buf1[2] = {4, 5};
const uint8_t WebSocketWriteReqTest::m_buf2[4] = {6, 7, 8, 9};

TEST_F(WebSocketWriteReqTest, ContinueDone) {
  req->m_continueBufPos = 3;
  ASSERT_EQ(req->Continue(stream, req), 0);
}

TEST_F(WebSocketWriteReqTest, ContinueTryWriteComplete) {
  EXPECT_CALL(stream, TryWrite(wpi::SpanEq(req->m_frames.m_bufs)))
      .WillOnce(Return(9));
  ASSERT_EQ(req->Continue(stream, req), 0);
}

TEST_F(WebSocketWriteReqTest, ContinueTryWriteNoProgress) {
  // if TryWrite returns 0
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(0));
  // Write should get called for all of next frame - make forward progress
  uv::Buffer remBufs[2] = {uv::Buffer{m_buf0}, uv::Buffer{m_buf1}};
  EXPECT_CALL(stream,
              DoWrite(wpi::SpanEq(std::span<const uv::Buffer>(remBufs)), _));
  ASSERT_EQ(req->Continue(stream, req), 1);
}

TEST_F(WebSocketWriteReqTest, ContinueTryWriteError) {
  // if TryWrite returns -1, the error is passed along
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(-1));
  ASSERT_EQ(req->Continue(stream, req), -1);
}

TEST_F(WebSocketWriteReqTest, ContinueTryWritePartialMidFrameMidBuf1) {
  // stop partway through buf 0
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(2));
  // Write should get called for remainder of buf 0 and all of buf 1
  uv::Buffer remBufs[2] = {uv::Buffer{&m_buf0[2], 1}, uv::Buffer{m_buf1}};
  EXPECT_CALL(stream,
              DoWrite(wpi::SpanEq(std::span<const uv::Buffer>(remBufs)), _));
  ASSERT_EQ(req->Continue(stream, req), 1);
}

TEST_F(WebSocketWriteReqTest, ContinueTryWritePartialMidFrameBufBoundary) {
  // stop at end of buf 0
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(3));
  // Write should get called for all of buf 1
  uv::Buffer remBufs[1] = {uv::Buffer{m_buf1}};
  EXPECT_CALL(stream,
              DoWrite(wpi::SpanEq(std::span<const uv::Buffer>(remBufs)), _));
  ASSERT_EQ(req->Continue(stream, req), 1);
}

TEST_F(WebSocketWriteReqTest, ContinueTryWritePartialMidFrameMidBuf2) {
  // stop partway through buf 1
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(4));
  // Write should get called for remainder of buf 1
  uv::Buffer remBufs[1] = {uv::Buffer{&m_buf1[1], 1}};
  EXPECT_CALL(stream,
              DoWrite(wpi::SpanEq(std::span<const uv::Buffer>(remBufs)), _));
  ASSERT_EQ(req->Continue(stream, req), 1);
}

TEST_F(WebSocketWriteReqTest, ContinueTryWritePartialFrameBoundary) {
  // stop at end of buf 1
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(5));
  // Write should get called for all of next frame
  uv::Buffer remBufs[1] = {uv::Buffer{m_buf2}};
  EXPECT_CALL(stream,
              DoWrite(wpi::SpanEq(std::span<const uv::Buffer>(remBufs)), _));
  ASSERT_EQ(req->Continue(stream, req), 1);
}

TEST_F(WebSocketWriteReqTest, ContinueTryWritePartialMidFrameMidBuf3) {
  // stop partway through buf 2
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(6));
  // Write should get called for remainder of buf 2
  uv::Buffer remBufs[1] = {uv::Buffer{&m_buf2[1], 3}};
  EXPECT_CALL(stream,
              DoWrite(wpi::SpanEq(std::span<const uv::Buffer>(remBufs)), _));
  ASSERT_EQ(req->Continue(stream, req), 1);
}

class WebSocketTrySendTest : public ::testing::Test {
 public:
  ::testing::StrictMock<MockStream> stream;
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
    WebSocket::Frame{WebSocket::Frame::kBinaryFragment, m_frame0bufs},
    WebSocket::Frame{WebSocket::Frame::kBinary, m_frame1bufs},
    WebSocket::Frame{WebSocket::Frame::kText, m_frame2bufs},
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
  ASSERT_THAT(
      TrySendFrames(
          true, stream, m_frames,
          [&](std::function<void(std::span<uv::Buffer>, uv::Error)>&& cb) {
            ++makeReqCalled;
            req = std::make_shared<MockWebSocketWriteReq>(std::move(cb));
            return req;
          },
          [&](auto bufs, auto err) {
            ++callbackCalled;
            ASSERT_THAT(bufs,
                        SpanEq(std::span<const uv::Buffer>(expectCbBufs)));
            ASSERT_EQ(err.code(), expectErr);
          }),
      SpanEq(expectRet));
}

TEST_F(WebSocketTrySendTest, ServerComplete) {
  // if trywrite sends everything
  EXPECT_CALL(stream, TryWrite(_))
      .WillOnce(Return(m_serialized[0].size() + m_serialized[1].size() +
                       m_serialized[2].size()));
  // return nothing, and call callback immediately
  CheckTrySendFrames(m_bufs, {});
  ASSERT_EQ(makeReqCalled, 0);
  ASSERT_EQ(callbackCalled, 1);
}

TEST_F(WebSocketTrySendTest, ServerNoProgress) {
  // if trywrite sends nothing
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(0));
  // we should get all the frames back (the callback may be called with an empty
  // set of buffers)
  CheckTrySendFrames({}, m_frames);
  ASSERT_EQ(makeReqCalled, 0);
  ASSERT_THAT(callbackCalled, AnyOf(0, 1));
}

TEST_F(WebSocketTrySendTest, ServerError) {
  // if TryWrite returns -1, the error is passed along
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(-1));
  CheckTrySendFrames(m_bufs, m_frames, -1);
  ASSERT_EQ(makeReqCalled, 0);
  ASSERT_EQ(callbackCalled, 1);
}

TEST_F(WebSocketTrySendTest, ServerPartialMidFrameMidBuf0) {
  // stop partway through buf 0 (first buf of frame 0)
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(m_frameHeaders[0].len + 2));
  // Write should get called for remainder of buf 0 and all of buf 1
  // buf 2 should get put into continuation because frame 0 is a fragment
  // return will be frame 2 only
  std::array<uv::Buffer, 2> remBufs{std::span{m_buf0data}.subspan(2),
                                    m_bufs[1]};
  std::array<uv::Buffer, 2> contBufs{m_frameHeaders[1], m_bufs[2]};
  std::array<int, 1> contFrameOffs{static_cast<int>(m_serialized[1].size())};
  EXPECT_CALL(stream, DoWrite(wpi::SpanEq(remBufs), _));
  CheckTrySendFrames({}, std::span{m_frames}.subspan(2));
  ASSERT_EQ(makeReqCalled, 1);
  ASSERT_THAT(req->m_frames.m_bufs, SpanEq(contBufs));
  ASSERT_EQ(req->m_continueBufPos, 0u);
  ASSERT_EQ(req->m_continueFramePos, 0u);
  ASSERT_THAT(req->m_continueFrameOffs, SpanEq(contFrameOffs));
  ASSERT_EQ(callbackCalled, 0);
}

TEST_F(WebSocketTrySendTest, ServerPartialMidFrameBufBoundary) {
  // stop at end of buf 0 (first buf of frame 0)
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(m_frameHeaders[0].len + 3));
  // Write should get called for all of buf 1
  // buf 2 should get put into continuation because frame 0 is a fragment
  // return will be frame 2 only
  std::array<uv::Buffer, 1> remBufs{m_bufs[1]};
  std::array<uv::Buffer, 2> contBufs{m_frameHeaders[1], m_bufs[2]};
  EXPECT_CALL(stream, DoWrite(wpi::SpanEq(remBufs), _));
  CheckTrySendFrames({}, std::span{m_frames}.subspan(2));
  ASSERT_EQ(makeReqCalled, 1);
  ASSERT_THAT(req->m_frames.m_bufs, SpanEq(contBufs));
  ASSERT_EQ(callbackCalled, 0);
}

TEST_F(WebSocketTrySendTest, ServerPartialMidFrameMidBuf1) {
  // stop partway through buf 1 (second buf of frame 0)
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(m_frameHeaders[0].len + 4));
  // Write should get called for remainder of buf 1
  // buf 2 should get put into continuation because frame 0 is a fragment
  // return will be frame 2 only
  std::array<uv::Buffer, 1> remBufs{std::span{m_buf1data}.subspan(1)};
  std::array<uv::Buffer, 2> contBufs{m_frameHeaders[1], m_bufs[2]};
  EXPECT_CALL(stream, DoWrite(wpi::SpanEq(remBufs), _));
  CheckTrySendFrames({}, std::span{m_frames}.subspan(2));
  ASSERT_EQ(makeReqCalled, 1);
  ASSERT_THAT(req->m_frames.m_bufs, SpanEq(contBufs));
  ASSERT_EQ(callbackCalled, 0);
}

TEST_F(WebSocketTrySendTest, ServerPartialFrameBoundary) {
  // stop at end of buf 1 (end of frame 0)
  EXPECT_CALL(stream, TryWrite(_))
      .WillOnce(Return(m_frameHeaders[0].len + m_frameHeaders[1].len + 5));
  // Write should get called for all of buf 2 because frame 0 is a fragment
  // no continuation
  // return will be frame 2 only
  std::array<uv::Buffer, 1> remBufs{m_bufs[2]};
  EXPECT_CALL(stream, DoWrite(wpi::SpanEq(remBufs), _));
  CheckTrySendFrames({}, std::span{m_frames}.subspan(2));
  ASSERT_EQ(makeReqCalled, 1);
  ASSERT_TRUE(req->m_frames.m_bufs.empty());
  ASSERT_EQ(callbackCalled, 0);
}

TEST_F(WebSocketTrySendTest, ServerPartialMidFrameMidBuf2) {
  // stop partway through buf 2 (frame 1 buf)
  EXPECT_CALL(stream, TryWrite(_))
      .WillOnce(Return(m_frameHeaders[0].len + m_frameHeaders[1].len + 6));
  // Write should get called for remainder of buf 2; no continuation
  // return will be frame 2 only
  std::array<uv::Buffer, 1> remBufs{std::span{m_buf2data}.subspan(1)};
  EXPECT_CALL(stream, DoWrite(wpi::SpanEq(remBufs), _));
  CheckTrySendFrames({}, std::span{m_frames}.subspan(2));
  ASSERT_EQ(makeReqCalled, 1);
  ASSERT_TRUE(req->m_frames.m_bufs.empty());
  ASSERT_EQ(callbackCalled, 0);
}

TEST_F(WebSocketTrySendTest, ServerFrameBoundary) {
  // stop at end of buf 2 (end of frame 1)
  EXPECT_CALL(stream, TryWrite(_))
      .WillOnce(Return(m_frameHeaders[0].len + m_frameHeaders[1].len + 9));
  // call callback immediately for bufs 0-2 and return frame 2
  CheckTrySendFrames(std::span{m_bufs}.subspan(0, 3),
                     std::span{m_frames}.subspan(2));
  ASSERT_EQ(makeReqCalled, 0);
  ASSERT_EQ(callbackCalled, 1);
}

TEST_F(WebSocketTrySendTest, ServerPartialLastFrame) {
  // stop partway through buf 3
  EXPECT_CALL(stream, TryWrite(_))
      .WillOnce(Return(m_frameHeaders[0].len + m_frameHeaders[1].len +
                       m_frameHeaders[2].len + 10));
  // Write called for remainder of buf 3; no continuation
  std::array<uv::Buffer, 1> remBufs{std::span{m_buf3data}.subspan(1)};
  EXPECT_CALL(stream, DoWrite(wpi::SpanEq(remBufs), _));
  CheckTrySendFrames({}, {});
  ASSERT_EQ(makeReqCalled, 1);
  ASSERT_TRUE(req->m_frames.m_bufs.empty());
  ASSERT_EQ(callbackCalled, 0);
}

TEST_F(WebSocketTrySendTest, Big) {
  std::vector<uv::Buffer> bufs;
  for (int i = 0; i < 100000;) {
    i += 1430;
    bufs.emplace_back(
        uv::Buffer::Allocate(i < 100000 ? 1430 : (100000 - (i - 1430))));
  }
  WebSocket::Frame frame{WebSocket::kOpBinary | WebSocket::kFlagFin, bufs};
  EXPECT_CALL(stream, TryWrite(_)).WillOnce(Return(7681));

  // Write called for remainder of buffers
  std::vector<uv::Buffer> remBufs;
  remBufs.emplace_back(bufs[5]);
  remBufs.back().base += 521;
  remBufs.back().len -= 521;
  for (size_t i = 6; i < bufs.size(); ++i) {
    remBufs.emplace_back(bufs[i]);
  }
  EXPECT_CALL(stream, DoWrite(wpi::SpanEq(remBufs), _));

  ASSERT_TRUE(
      TrySendFrames(
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
  ASSERT_EQ(makeReqCalled, 1);
  ASSERT_TRUE(req->m_frames.m_bufs.empty());
  ASSERT_EQ(callbackCalled, 0);
}

}  // namespace wpi::detail
