/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "support/raw_istream.h"

#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

using namespace wpi;

void raw_mem_istream::close() {}

std::size_t raw_mem_istream::in_avail() const { return m_left; }

void raw_mem_istream::read_impl(void* data, std::size_t len) {
  if (len > m_left) {
    error_detected();
    return;
  }
  std::memcpy(data, m_cur, len);
  m_cur += len;
  m_left -= len;
}

raw_fd_istream::raw_fd_istream(int fd, bool shouldClose, std::size_t bufSize)
    : m_bufSize(bufSize), m_fd(fd), m_shouldClose(shouldClose) {
  m_cur = m_end = m_buf = static_cast<char*>(std::malloc(bufSize));
}

raw_fd_istream::~raw_fd_istream() {
  if (m_shouldClose) close();
  std::free(m_buf);
}

void raw_fd_istream::close() {
  if (m_fd >= 0) {
    ::close(m_fd);
    m_fd = -1;
  }
}

std::size_t raw_fd_istream::in_avail() const { return m_end - m_cur; }

void raw_fd_istream::read_impl(void* data, std::size_t len) {
  std::size_t left = m_end - m_cur;
  if (left < len) {
    // not enough data
    if (m_cur == m_end) {
#ifdef _WIN32
      int count = ::_read(m_fd, m_buf, m_bufSize);
#else
      ssize_t count = ::read(m_fd, m_buf, m_bufSize);
#endif
      if (count < 0) {
        error_detected();
        return;
      }
      m_cur = m_buf;
      m_end = m_buf + count;
      return read_impl(data, len);
    }

    std::memcpy(data, m_cur, left);
    return read_impl(static_cast<char*>(data) + left, len - left);
  }

  std::memcpy(data, m_cur, len);
  m_cur += len;
}
