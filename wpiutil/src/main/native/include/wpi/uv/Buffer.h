/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_BUFFER_H_
#define WPIUTIL_WPI_UV_BUFFER_H_

#include <uv.h>

#include <cstring>
#include <initializer_list>
#include <utility>

#include "wpi/ArrayRef.h"
#include "wpi/StringRef.h"

namespace wpi {
namespace uv {

/**
 * Data buffer.  Convenience wrapper around uv_buf_t.
 */
class Buffer : public uv_buf_t {
 public:
  Buffer() {
    base = nullptr;
    len = 0;
  }
  /*implicit*/ Buffer(const uv_buf_t& oth) {  // NOLINT(runtime/explicit)
    base = oth.base;
    len = oth.len;
  }
  /*implicit*/ Buffer(StringRef str)  // NOLINT(runtime/explicit)
      : Buffer{str.data(), str.size()} {}
  /*implicit*/ Buffer(ArrayRef<uint8_t> arr)  // NOLINT(runtime/explicit)
      : Buffer{reinterpret_cast<const char*>(arr.data()), arr.size()} {}
  Buffer(char* base_, size_t len_) {
    base = base_;
    len = len_;
  }
  Buffer(const char* base_, size_t len_) {
    base = const_cast<char*>(base_);
    len = len_;
  }

  ArrayRef<char> data() const { return ArrayRef<char>{base, len}; }
  MutableArrayRef<char> data() { return MutableArrayRef<char>{base, len}; }

  operator ArrayRef<char>() const { return data(); }
  operator MutableArrayRef<char>() { return data(); }

  static Buffer Allocate(size_t size) { return Buffer{new char[size], size}; }

  static Buffer Dup(StringRef in) {
    Buffer buf = Allocate(in.size());
    std::memcpy(buf.base, in.data(), in.size());
    return buf;
  }

  static Buffer Dup(ArrayRef<uint8_t> in) {
    Buffer buf = Allocate(in.size());
    std::memcpy(buf.base, in.begin(), in.size());
    return buf;
  }

  void Deallocate() {
    delete[] base;
    base = nullptr;
    len = 0;
  }

  Buffer Move() {
    Buffer buf = *this;
    base = nullptr;
    len = 0;
    return buf;
  }

  friend void swap(Buffer& a, Buffer& b) {
    using std::swap;
    swap(a.base, b.base);
    swap(a.len, b.len);
  }
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_BUFFER_H_
