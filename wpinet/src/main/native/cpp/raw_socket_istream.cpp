// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/raw_socket_istream.h"

#include "wpi/NetworkStream.h"

using namespace wpi;

void raw_socket_istream::read_impl(void* data, size_t len) {
  char* cdata = static_cast<char*>(data);
  size_t pos = 0;

  while (pos < len) {
    NetworkStream::Error err;
    size_t count = m_stream.receive(&cdata[pos], len - pos, &err, m_timeout);
    if (count == 0) {
      error_detected();
      break;
    }
    pos += count;
  }
  set_read_count(pos);
}

void raw_socket_istream::close() {
  m_stream.close();
}

size_t raw_socket_istream::in_avail() const {
  return 0;
}
