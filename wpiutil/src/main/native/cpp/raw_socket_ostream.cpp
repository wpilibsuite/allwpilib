// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/raw_socket_ostream.h"

#include "wpi/NetworkStream.h"

using namespace wpi;

raw_socket_ostream::~raw_socket_ostream() {
  flush();
  if (m_shouldClose) {
    close();
  }
}

void raw_socket_ostream::write_impl(const char* data, size_t len) {
  size_t pos = 0;

  while (pos < len) {
    NetworkStream::Error err;
    size_t count = m_stream.send(&data[pos], len - pos, &err);
    if (count == 0) {
      error_detected();
      return;
    }
    pos += count;
  }
}

uint64_t raw_socket_ostream::current_pos() const {
  return 0;
}

void raw_socket_ostream::close() {
  if (!m_shouldClose) {
    return;
  }
  flush();
  m_stream.close();
}
