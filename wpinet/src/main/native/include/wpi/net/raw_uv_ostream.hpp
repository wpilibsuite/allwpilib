// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_RAW_UV_OSTREAM_HPP_
#define WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_RAW_UV_OSTREAM_HPP_

#include <functional>
#include <span>
#include <utility>

#include "wpi/net/uv/Buffer.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/raw_ostream.hpp"

namespace wpi::net {

/**
 * wpi::util::raw_ostream style output to a wpi::util::SmallVector of uv::Buffer
 * buffers.  Fixed-size buffers are allocated and appended as necessary to fit
 * the data being output. The wpi::util::SmallVector need not be empty at start.
 */
class raw_uv_ostream : public wpi::util::raw_ostream {
 public:
  /**
   * Construct a new raw_uv_ostream.
   * @param bufs Buffers vector.  NOT cleared on construction.
   * @param allocSize Size to allocate for each buffer; allocation will be
   *                  performed using Buffer::Allocate().
   */
  raw_uv_ostream(wpi::util::SmallVectorImpl<uv::Buffer>& bufs, size_t allocSize)
      : m_bufs(bufs), m_alloc([=] { return uv::Buffer::Allocate(allocSize); }) {
    SetUnbuffered();
  }

  /**
   * Construct a new raw_uv_ostream.
   * @param bufs Buffers vector.  NOT cleared on construction.
   * @param alloc Allocator.
   */
  raw_uv_ostream(wpi::util::SmallVectorImpl<uv::Buffer>& bufs,
                 std::function<uv::Buffer()> alloc)
      : m_bufs(bufs), m_alloc(std::move(alloc)) {
    SetUnbuffered();
  }

  ~raw_uv_ostream() override = default;

  /**
   * Returns an span to the buffers.
   */
  std::span<uv::Buffer> bufs() { return m_bufs; }

  void flush() = delete;

  /**
   * Resets the amount of allocated space.
   */
  void reset() { m_left = 0; }

 private:
  void write_impl(const char* data, size_t len) override;
  uint64_t current_pos() const override;

  wpi::util::SmallVectorImpl<uv::Buffer>& m_bufs;
  std::function<uv::Buffer()> m_alloc;

  // How much allocated space is left in the current buffer.
  size_t m_left = 0;
};

}  // namespace wpi::net

#endif  // WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_RAW_UV_OSTREAM_HPP_
