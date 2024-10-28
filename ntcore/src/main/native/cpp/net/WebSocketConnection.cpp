// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WebSocketConnection.h"

#include <algorithm>
#include <span>

#include <wpi/Endian.h>
#include <wpi/Logger.h>
#include <wpi/SpanExtras.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>
#include <wpinet/WebSocket.h>
#include <wpinet/raw_uv_ostream.h>

using namespace nt;
using namespace nt::net;

// MTU - assume Ethernet, IPv6, TCP; does not include WS frame header (max 10)
static constexpr size_t kMTU = 1500 - 40 - 20;
static constexpr size_t kAllocSize = kMTU - 10;
// leave enough room for a "typical" message size so we don't create lots of
// fragmented frames
static constexpr size_t kNewFrameThresholdBytes = kAllocSize - 50;
static constexpr size_t kFlushThresholdFrames = 32;
static constexpr size_t kFlushThresholdBytes = 16384;
static constexpr size_t kMaxPoolSize = 32;

class WebSocketConnection::Stream final : public wpi::raw_ostream {
 public:
  explicit Stream(WebSocketConnection& conn) : m_conn{conn} {
    auto& buf = conn.m_bufs.back();
    SetBuffer(buf.base, kAllocSize);
    SetNumBytesInBuffer(buf.len);
  }

  ~Stream() final {
    m_disableAlloc = true;
    flush();
  }

 private:
  size_t preferred_buffer_size() const final { return 0; }
  void write_impl(const char* data, size_t len) final;
  uint64_t current_pos() const final { return m_conn.m_framePos; }

  WebSocketConnection& m_conn;
  bool m_disableAlloc = false;
};

void WebSocketConnection::Stream::write_impl(const char* data, size_t len) {
  if (data == m_conn.m_bufs.back().base) {
    // flush_nonempty() case
    size_t amt = len - m_conn.m_bufs.back().len;
    WPI_DEBUG4(m_conn.m_logger, "conn: writing {} bytes (nonempty)", amt);
    m_conn.m_bufs.back().len = len;
    m_conn.m_framePos += amt;
    m_conn.m_written += amt;
    if (!m_disableAlloc) {
#ifdef NT_ENABLE_WS_FRAG
      m_conn.m_frames.back().opcode &= ~wpi::WebSocket::kFlagFin;
      m_conn.StartFrame(wpi::WebSocket::Frame::kFragment);
#else
      m_conn.m_bufs.emplace_back(m_conn.AllocBuf());
      m_conn.m_bufs.back().len = 0;
      ++m_conn.m_frames.back().end;
#endif
      SetBuffer(m_conn.m_bufs.back().base, kAllocSize);
    }
    return;
  }

  bool updateBuffer = false;
  while (len > 0) {
    auto& buf = m_conn.m_bufs.back();
    assert(buf.len <= kAllocSize);
    size_t amt = (std::min)(static_cast<int>(kAllocSize - buf.len),
                            static_cast<int>(len));
    if (amt > 0) {
      WPI_DEBUG4(m_conn.m_logger, "conn: writing {} bytes", amt);
      std::memcpy(buf.base + buf.len, data, amt);
      buf.len += amt;
      m_conn.m_framePos += amt;
      m_conn.m_written += amt;
      data += amt;
      len -= amt;
    }
    if (buf.len >= kAllocSize && (len > 0 || !m_disableAlloc)) {
#ifdef NT_ENABLE_WS_FRAG
      // fragment the current frame and start a new one
      m_conn.m_frames.back().opcode &= ~wpi::WebSocket::kFlagFin;
      m_conn.StartFrame(wpi::WebSocket::Frame::kFragment);
#else
      m_conn.m_bufs.emplace_back(m_conn.AllocBuf());
      m_conn.m_bufs.back().len = 0;
      ++m_conn.m_frames.back().end;
#endif
      updateBuffer = true;
    }
  }

  if (updateBuffer) {
    SetBuffer(m_conn.m_bufs.back().base, kAllocSize);
  }
}

WebSocketConnection::WebSocketConnection(wpi::WebSocket& ws,
                                         unsigned int version,
                                         wpi::Logger& logger)
    : m_ws{ws}, m_logger{logger}, m_version{version} {}

WebSocketConnection::~WebSocketConnection() {
  for (auto&& buf : m_bufs) {
    buf.Deallocate();
  }
  for (auto&& buf : m_buf_pool) {
    buf.Deallocate();
  }
}

void WebSocketConnection::SendPing(uint64_t time) {
  WPI_DEBUG4(m_logger, "conn: sending ping {}", time);
  auto buf = AllocBuf();
  buf.len = 8;
  wpi::support::endian::write64<wpi::endianness::native>(buf.base, time);
  m_ws.SendPing({buf}, [selfweak = weak_from_this()](auto bufs, auto err) {
    if (auto self = selfweak.lock()) {
      self->m_err = err;
      self->ReleaseBufs(bufs);
    } else {
      for (auto&& buf : bufs) {
        buf.Deallocate();
      }
    }
  });
}

void WebSocketConnection::StartFrame(uint8_t opcode) {
  WPI_DEBUG4(m_logger, "conn: starting frame {}",
             static_cast<unsigned int>(opcode));
  m_frames.emplace_back(opcode, m_bufs.size(), m_bufs.size() + 1);
  m_bufs.emplace_back(AllocBuf());
  m_bufs.back().len = 0;
}

void WebSocketConnection::FinishText() {
  assert(!m_bufs.empty());
  auto& buf = m_bufs.back();
  assert(buf.len < (kAllocSize + 1));  // safe because we alloc one more byte
  buf.base[buf.len++] = ']';
}

int WebSocketConnection::Write(
    State kind, wpi::function_ref<void(wpi::raw_ostream& os)> writer) {
  bool first = false;
  if (m_state != kind ||
      (m_state == kind && m_framePos >= kNewFrameThresholdBytes)) {
    // start a new frame
    if (m_state == kText) {
      FinishText();
    }
    m_state = kind;
    if (!m_frames.empty()) {
      m_frames.back().opcode |= wpi::WebSocket::kFlagFin;
    }
    StartFrame(m_state == kText ? wpi::WebSocket::Frame::kText
                                : wpi::WebSocket::Frame::kBinary);
    m_framePos = 0;
    first = true;
  }
  {
    Stream os{*this};
    if (kind == kText) {
      os << (first ? '[' : ',');
    }
    WPI_DEBUG4(m_logger, "writing");
    writer(os);
  }
  ++m_frames.back().count;
  if (m_frames.size() > kFlushThresholdFrames ||
      m_written >= kFlushThresholdBytes) {
    return Flush();
  }
  return 0;
}

int WebSocketConnection::Flush() {
  WPI_DEBUG4(m_logger, "conn: flushing");
  m_lastFlushTime = wpi::Now();
  if (m_state == kEmpty) {
    return 0;
  }
  if (m_state == kText) {
    FinishText();
  }
  m_state = kEmpty;
  m_written = 0;

  if (m_frames.empty()) {
    return 0;
  }
  m_frames.back().opcode |= wpi::WebSocket::kFlagFin;

  // convert internal frames into WS frames
  m_ws_frames.clear();
  m_ws_frames.reserve(m_frames.size());
  for (auto&& frame : m_frames) {
    m_ws_frames.emplace_back(
        frame.opcode,
        std::span{m_bufs}.subspan(frame.start, frame.end - frame.start));
  }

  auto unsentFrames = m_ws.TrySendFrames(
      m_ws_frames, [selfweak = weak_from_this()](auto bufs, auto err) {
        if (auto self = selfweak.lock()) {
          self->m_err = err;
          self->ReleaseBufs(bufs);
        } else {
          for (auto&& buf : bufs) {
            buf.Deallocate();
          }
        }
      });
  m_ws_frames.clear();
  if (m_err) {
    m_frames.clear();
    m_bufs.clear();
    return m_err.code();
  }

  int count = 0;
  for (auto&& frame :
       wpi::take_back(std::span{m_frames}, unsentFrames.size())) {
    ReleaseBufs(
        std::span{m_bufs}.subspan(frame.start, frame.end - frame.start));
    count += frame.count;
  }
  m_frames.clear();
  m_bufs.clear();
  return count;
}

void WebSocketConnection::Send(
    uint8_t opcode, wpi::function_ref<void(wpi::raw_ostream& os)> writer) {
  wpi::SmallVector<wpi::uv::Buffer, 4> bufs;
  wpi::raw_uv_ostream os{bufs, [this] { return AllocBuf(); }};
  if (opcode == wpi::WebSocket::Frame::kText) {
    os << '[';
  }
  writer(os);
  if (opcode == wpi::WebSocket::Frame::kText) {
    os << ']';
  }
  wpi::WebSocket::Frame frame{opcode, os.bufs()};
  WPI_DEBUG4(m_logger, "Send({})", static_cast<uint8_t>(opcode));
  m_ws.SendFrames({{frame}}, [selfweak = weak_from_this()](auto bufs, auto) {
    if (auto self = selfweak.lock()) {
      self->ReleaseBufs(bufs);
    } else {
      for (auto&& buf : bufs) {
        buf.Deallocate();
      }
    }
  });
}

void WebSocketConnection::Disconnect(std::string_view reason) {
  m_reason = reason;
  m_ws.Fail(1001, reason);
}

wpi::uv::Buffer WebSocketConnection::AllocBuf() {
  if (!m_buf_pool.empty()) {
    auto buf = m_buf_pool.back();
    m_buf_pool.pop_back();
    return buf;
  }
  return wpi::uv::Buffer::Allocate(kAllocSize + 1);  // leave space for ']'
}

void WebSocketConnection::ReleaseBufs(std::span<wpi::uv::Buffer> bufs) {
#ifdef __SANITIZE_ADDRESS__
  size_t numToPool = 0;
#else
  size_t numToPool = (std::min)(bufs.size(), kMaxPoolSize - m_buf_pool.size());
  m_buf_pool.insert(m_buf_pool.end(), bufs.begin(), bufs.begin() + numToPool);
#endif
  for (auto&& buf : bufs.subspan(numToPool)) {
    buf.Deallocate();
  }
}
