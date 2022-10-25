// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WebSocketConnection.h"

#include <span>

#include <wpi/SpanExtras.h>
#include <wpinet/WebSocket.h>

using namespace nt;
using namespace nt::net;

static constexpr size_t kAllocSize = 4096;
static constexpr size_t kTextFrameRolloverSize = 4096;
static constexpr size_t kBinaryFrameRolloverSize = 8192;

WebSocketConnection::WebSocketConnection(wpi::WebSocket& ws)
    : m_ws{ws},
      m_text_os{m_text_buffers, [this] { return AllocBuf(); }},
      m_binary_os{m_binary_buffers, [this] { return AllocBuf(); }} {}

WebSocketConnection::~WebSocketConnection() {
  for (auto&& buf : m_buf_pool) {
    buf.Deallocate();
  }
}

void WebSocketConnection::Flush() {
  FinishSendText();
  FinishSendBinary();
  if (m_frames.empty()) {
    return;
  }

  // convert internal frames into WS frames
  m_ws_frames.clear();
  m_ws_frames.reserve(m_frames.size());
  for (auto&& frame : m_frames) {
    m_ws_frames.emplace_back(frame.opcode,
                             std::span{frame.bufs->begin() + frame.start,
                                       frame.bufs->begin() + frame.end});
  }

  ++m_sendsActive;
  m_ws.SendFrames(m_ws_frames, [selfweak = weak_from_this()](auto bufs, auto) {
    if (auto self = selfweak.lock()) {
      self->m_buf_pool.insert(self->m_buf_pool.end(), bufs.begin(), bufs.end());
      if (self->m_sendsActive > 0) {
        --self->m_sendsActive;
      }
    }
  });
  m_frames.clear();
  m_text_buffers.clear();
  m_binary_buffers.clear();
  m_text_pos = 0;
  m_binary_pos = 0;
}

void WebSocketConnection::Disconnect(std::string_view reason) {
  m_ws.Close(1005, reason);
}

void WebSocketConnection::StartSendText() {
  // limit amount per single frame
  size_t total = 0;
  for (size_t i = m_text_pos; i < m_text_buffers.size(); ++i) {
    total += m_text_buffers[i].len;
  }
  if (total >= kTextFrameRolloverSize) {
    FinishSendText();
  }

  if (m_in_text) {
    m_text_os << ',';
  } else {
    m_text_os << '[';
    m_in_text = true;
  }
}

void WebSocketConnection::FinishSendText() {
  if (m_in_text) {
    m_text_os << ']';
    m_in_text = false;
  }
  if (m_text_pos >= m_text_buffers.size()) {
    return;
  }
  m_frames.emplace_back(wpi::WebSocket::Frame::kText, &m_text_buffers,
                        m_text_pos, m_text_buffers.size());
  m_text_pos = m_text_buffers.size();
  m_text_os.reset();
}

void WebSocketConnection::StartSendBinary() {
  // limit amount per single frame
  size_t total = 0;
  for (size_t i = m_binary_pos; i < m_binary_buffers.size(); ++i) {
    total += m_binary_buffers[i].len;
  }
  if (total >= kBinaryFrameRolloverSize) {
    FinishSendBinary();
  }
}

void WebSocketConnection::FinishSendBinary() {
  if (m_binary_pos >= m_binary_buffers.size()) {
    return;
  }
  m_frames.emplace_back(wpi::WebSocket::Frame::kBinary, &m_binary_buffers,
                        m_binary_pos, m_binary_buffers.size());
  m_binary_pos = m_binary_buffers.size();
  m_binary_os.reset();
}

wpi::uv::Buffer WebSocketConnection::AllocBuf() {
  if (!m_buf_pool.empty()) {
    auto buf = m_buf_pool.back();
    m_buf_pool.pop_back();
    return buf;
  }
  return wpi::uv::Buffer::Allocate(kAllocSize);
}
