// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#define _CRT_NONSTDC_NO_WARNINGS

#include "wpi/util/raw_istream.hpp"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <cstdlib>
#include <cstring>
#include <string_view>

#include "wpi/util/SmallVector.hpp"
#include "wpi/util/fs.hpp"

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

std::string_view raw_istream::getline(SmallVectorImpl<char>& buf, int maxLen) {
  buf.clear();
  for (int i = 0; i < maxLen; ++i) {
    char c;
    read(c);
    if (has_error()) {
      return {buf.data(), buf.size()};
    }
    if (c == '\r') {
      continue;
    }
    buf.push_back(c);
    if (c == '\n') {
      break;
    }
  }
  return {buf.data(), buf.size()};
}

void raw_mem_istream::close() {}

size_t raw_mem_istream::in_avail() const {
  return m_left;
}

void raw_mem_istream::read_impl(void* data, size_t len) {
  if (len > m_left) {
    error_detected();
    len = m_left;
  }
  std::memcpy(data, m_cur, len);
  m_cur += len;
  m_left -= len;
  set_read_count(len);
}

static int getFD(std::string_view Filename, std::error_code& EC) {
  // Handle "-" as stdin. Note that when we do this, we consider ourself
  // the owner of stdin. This means that we can do things like close the
  // file descriptor when we're done and set the "binary" flag globally.
  if (Filename == "-") {
    EC = std::error_code();
    return STDIN_FILENO;
  }

  fs::file_t F = fs::OpenFileForRead(Filename, EC);
  if (EC) {
    return -1;
  }
  int FD = fs::FileToFd(F, EC, fs::OF_None);
  if (EC) {
    return -1;
  }
  return FD;
}

raw_fd_istream::raw_fd_istream(std::string_view filename, std::error_code& ec,
                               size_t bufSize)
    : raw_fd_istream(getFD(filename, ec), true, bufSize) {}

raw_fd_istream::raw_fd_istream(int fd, bool shouldClose, size_t bufSize)
    : m_bufSize(bufSize), m_fd(fd), m_shouldClose(shouldClose) {
  m_cur = m_end = m_buf = static_cast<char*>(std::malloc(bufSize));
}

raw_fd_istream::~raw_fd_istream() {
  if (m_shouldClose) {
    close();
  }
  std::free(m_buf);
}

void raw_fd_istream::close() {
  if (m_fd >= 0) {
    ::close(m_fd);
    m_fd = -1;
  }
}

size_t raw_fd_istream::in_avail() const {
  return m_end - m_cur;
}

void raw_fd_istream::read_impl(void* data, size_t len) {
  char* cdata = static_cast<char*>(data);
  size_t pos = 0;
  while (static_cast<size_t>(m_end - m_cur) < len) {
    // not enough data
    if (m_cur == m_end) {
#ifdef _WIN32
      int count = ::_read(m_fd, m_buf, m_bufSize);
#else
      ssize_t count = ::read(m_fd, m_buf, m_bufSize);
#endif
      if (count <= 0) {
        error_detected();
        set_read_count(pos);
        return;
      }
      m_cur = m_buf;
      m_end = m_buf + count;
      continue;
    }

    size_t left = m_end - m_cur;
    std::memcpy(&cdata[pos], m_cur, left);
    m_cur += left;
    pos += left;
    len -= left;
  }

  std::memcpy(&cdata[pos], m_cur, len);
  m_cur += len;
  pos += len;
  set_read_count(pos);
}
