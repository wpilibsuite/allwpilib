// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "UvStreamConnection3.h"

#include <wpinet/uv/Stream.h>

using namespace nt;
using namespace nt::net3;

UvStreamConnection3::UvStreamConnection3(wpi::uv::Stream& stream)
    : m_stream{stream}, m_os{m_buffers, [this] { return AllocBuf(); }} {}

UvStreamConnection3::~UvStreamConnection3() {
  for (auto&& buf : m_buf_pool) {
    buf.Deallocate();
  }
}

void UvStreamConnection3::Flush() {
  if (m_buffers.empty()) {
    return;
  }
  ++m_sendsActive;
  m_stream.Write(m_buffers, [selfweak = weak_from_this()](auto bufs, auto) {
    if (auto self = selfweak.lock()) {
      self->m_buf_pool.insert(self->m_buf_pool.end(), bufs.begin(), bufs.end());
      if (self->m_sendsActive > 0) {
        --self->m_sendsActive;
      }
    }
  });
  m_buffers.clear();
  m_os.reset();
}

void UvStreamConnection3::Disconnect(std::string_view reason) {
  m_reason = reason;
  m_stream.Close();
}

void UvStreamConnection3::FinishSend() {}

wpi::uv::Buffer UvStreamConnection3::AllocBuf() {
  if (!m_buf_pool.empty()) {
    auto buf = m_buf_pool.back();
    m_buf_pool.pop_back();
    return buf;
  }
  return wpi::uv::Buffer::Allocate(kAllocSize);
}
