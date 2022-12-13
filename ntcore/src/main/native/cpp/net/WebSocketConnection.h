// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <vector>

#include <wpi/SmallVector.h>
#include <wpinet/WebSocket.h>
#include <wpinet/raw_uv_ostream.h>
#include <wpinet/uv/Buffer.h>

#include "WireConnection.h"

namespace nt::net {

class WebSocketConnection final
    : public WireConnection,
      public std::enable_shared_from_this<WebSocketConnection> {
 public:
  explicit WebSocketConnection(wpi::WebSocket& ws);
  ~WebSocketConnection() override;
  WebSocketConnection(const WebSocketConnection&) = delete;
  WebSocketConnection& operator=(const WebSocketConnection&) = delete;

  bool Ready() const final { return m_sendsActive == 0; }

  TextWriter SendText() final { return {m_text_os, *this}; }
  BinaryWriter SendBinary() final { return {m_binary_os, *this}; }

  void Flush() final;

  void Disconnect(std::string_view reason) final;

 private:
  void StartSendText() final;
  void FinishSendText() final;
  void StartSendBinary() final;
  void FinishSendBinary() final;

  wpi::uv::Buffer AllocBuf();

  wpi::WebSocket& m_ws;
  // Can't use WS frames directly as span could have dangling pointers
  struct Frame {
    Frame(uint8_t opcode, wpi::SmallVectorImpl<wpi::uv::Buffer>* bufs,
          size_t start, size_t end)
        : opcode{opcode}, bufs{bufs}, start{start}, end{end} {}
    uint8_t opcode;
    wpi::SmallVectorImpl<wpi::uv::Buffer>* bufs;
    size_t start;
    size_t end;
  };
  std::vector<Frame> m_frames;
  std::vector<wpi::WebSocket::Frame> m_ws_frames;  // to reduce allocs
  wpi::SmallVector<wpi::uv::Buffer, 4> m_text_buffers;
  wpi::SmallVector<wpi::uv::Buffer, 4> m_binary_buffers;
  std::vector<wpi::uv::Buffer> m_buf_pool;
  wpi::raw_uv_ostream m_text_os;
  wpi::raw_uv_ostream m_binary_os;
  size_t m_text_pos = 0;
  size_t m_binary_pos = 0;
  bool m_in_text = false;
  int m_sendsActive = 0;
};

}  // namespace nt::net
