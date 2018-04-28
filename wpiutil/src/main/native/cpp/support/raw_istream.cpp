/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "support/raw_istream.h"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <cstdlib>
#include <cstring>

#include "llvm/FileSystem.h"
#include "llvm/SmallVector.h"
#include "llvm/StringRef.h"

#if defined(_MSC_VER)
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif
#endif

using namespace wpi;

llvm::StringRef raw_istream::getline(llvm::SmallVectorImpl<char>& buf,
                                     int maxLen) {
  buf.clear();
  for (int i = 0; i < maxLen; ++i) {
    char c;
    read(c);
    if (has_error()) return llvm::StringRef{buf.data(), buf.size()};
    if (c == '\r') continue;
    buf.push_back(c);
    if (c == '\n') break;
  }
  return llvm::StringRef{buf.data(), buf.size()};
}

raw_mem_istream::raw_mem_istream(llvm::StringRef mem)
    : raw_mem_istream(mem.data(), mem.size()) {}

void raw_mem_istream::close() {}

size_t raw_mem_istream::in_avail() const { return m_left; }

void raw_mem_istream::read_impl(void* data, size_t len) {
  if (len > m_left) {
    error_detected();
    return;
  }
  std::memcpy(data, m_cur, len);
  m_cur += len;
  m_left -= len;
}

static int getFD(const llvm::Twine& Filename, std::error_code& EC) {
  // Handle "-" as stdin. Note that when we do this, we consider ourself
  // the owner of stdin. This means that we can do things like close the
  // file descriptor when we're done and set the "binary" flag globally.
  if (Filename.isSingleStringRef() && Filename.getSingleStringRef() == "-") {
    EC = std::error_code();
    return STDIN_FILENO;
  }

  int FD;

  EC = llvm::sys::fs::openFileForRead(Filename, FD);
  if (EC) return -1;

  EC = std::error_code();
  return FD;
}

raw_fd_istream::raw_fd_istream(const llvm::Twine& filename, std::error_code& ec,
                               size_t bufSize)
    : raw_fd_istream(getFD(filename, ec), true, bufSize) {}

raw_fd_istream::raw_fd_istream(int fd, bool shouldClose, size_t bufSize)
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

size_t raw_fd_istream::in_avail() const { return m_end - m_cur; }

void raw_fd_istream::read_impl(void* data, size_t len) {
  size_t left = m_end - m_cur;
  if (left < len) {
    // not enough data
    if (m_cur == m_end) {
#ifdef _WIN32
      int count = ::_read(m_fd, m_buf, m_bufSize);
#else
      ssize_t count = ::read(m_fd, m_buf, m_bufSize);
#endif
      if (count <= 0) {
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
