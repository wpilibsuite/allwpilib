/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_RAW_UV_OSTREAM_H_
#define WPIUTIL_WPI_RAW_UV_OSTREAM_H_

#include "wpi/ArrayRef.h"
#include "wpi/SmallVector.h"
#include "wpi/raw_ostream.h"
#include "wpi/uv/Buffer.h"

namespace wpi {

/**
 * raw_ostream style output to a SmallVector of uv::Buffer buffers.  Fixed-size
 * buffers are allocated and appended as necessary to fit the data being output.
 * The SmallVector need not be empty at start.
 */
class raw_uv_ostream : public raw_ostream {
 public:
  /**
   * Construct a new raw_uv_ostream.
   * @param bufs Buffers vector.  NOT cleared on construction.
   * @param allocSize Size to allocate for each buffer; allocation will be
   *                  performed using Buffer::Allocate().
   */
  raw_uv_ostream(SmallVectorImpl<uv::Buffer>& bufs, size_t allocSize)
      : m_bufs(bufs),
        m_alloc([=]() { return uv::Buffer::Allocate(allocSize); }) {
    SetUnbuffered();
  }

  /**
   * Construct a new raw_uv_ostream.
   * @param bufs Buffers vector.  NOT cleared on construction.
   * @param alloc Allocator.
   */
  raw_uv_ostream(SmallVectorImpl<uv::Buffer>& bufs,
                 std::function<uv::Buffer()> alloc)
      : m_bufs(bufs), m_alloc(alloc) {
    SetUnbuffered();
  }

  ~raw_uv_ostream() override = default;

  /**
   * Returns an ArrayRef to the buffers.
   */
  ArrayRef<uv::Buffer> bufs() { return m_bufs; }

  void flush() = delete;

 private:
  void write_impl(const char* data, size_t len) override;
  uint64_t current_pos() const override;

  SmallVectorImpl<uv::Buffer>& m_bufs;
  std::function<uv::Buffer()> m_alloc;

  // How much allocated space is left in the current buffer.
  size_t m_left = 0;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_RAW_UV_OSTREAM_H_
