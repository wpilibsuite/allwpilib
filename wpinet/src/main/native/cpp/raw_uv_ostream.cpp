// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/raw_uv_ostream.h"

#include <cstring>

using namespace wpi;

void raw_uv_ostream::write_impl(const char* data, size_t len) {
  while (len > 0) {
    // allocate additional buffers as required
    if (m_left == 0) {
      m_bufs.emplace_back(m_alloc());
      // we want bufs() to always be valid, so set len=0 and keep track of the
      // amount of space remaining separately
      m_left = m_bufs.back().len;
      m_bufs.back().len = 0;
      assert(m_left != 0);
    }

    size_t amt = (std::min)(m_left, len);
    auto& buf = m_bufs.back();
    std::memcpy(buf.base + buf.len, data, amt);
    data += amt;
    len -= amt;
    buf.len += amt;
    m_left -= amt;
  }
}

uint64_t raw_uv_ostream::current_pos() const {
  uint64_t size = 0;
  for (auto&& buf : m_bufs) {
    size += buf.len;
  }
  return size;
}
