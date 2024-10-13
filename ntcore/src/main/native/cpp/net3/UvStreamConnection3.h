// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/SmallVector.h>
#include <wpinet/raw_uv_ostream.h>
#include <wpinet/uv/Buffer.h>
#include <wpinet/uv/Stream.h>

#include "net3/WireConnection3.h"

namespace wpi::uv {
class Stream;
}  // namespace wpi::uv

namespace nt::net3 {

class UvStreamConnection3 final
    : public WireConnection3,
      public std::enable_shared_from_this<UvStreamConnection3> {
  static constexpr size_t kAllocSize = 4096;

 public:
  explicit UvStreamConnection3(wpi::uv::Stream& stream);
  ~UvStreamConnection3() override;
  UvStreamConnection3(const UvStreamConnection3&) = delete;
  UvStreamConnection3& operator=(const UvStreamConnection3&) = delete;

  bool Ready() const final { return m_sendsActive == 0; }

  Writer Send() final { return {m_os, *this}; }

  void Flush() final;

  uint64_t GetLastFlushTime() const final { return m_lastFlushTime; }

  void StopRead() final {
    if (m_readActive) {
      m_stream.StopRead();
      m_readActive = false;
    }
  }
  void StartRead() final {
    if (!m_readActive) {
      m_stream.StartRead();
      m_readActive = true;
    }
  }

  void Disconnect(std::string_view reason) final;

  std::string_view GetDisconnectReason() const { return m_reason; }

  wpi::uv::Stream& GetStream() { return m_stream; }

 private:
  void FinishSend() final;

  wpi::uv::Buffer AllocBuf();

  wpi::uv::Stream& m_stream;
  wpi::SmallVector<wpi::uv::Buffer, 4> m_buffers;
  std::vector<wpi::uv::Buffer> m_buf_pool;
  wpi::raw_uv_ostream m_os;
  std::string m_reason;
  uint64_t m_lastFlushTime = 0;
  int m_sendsActive = 0;
  bool m_readActive = true;
};

}  // namespace nt::net3
