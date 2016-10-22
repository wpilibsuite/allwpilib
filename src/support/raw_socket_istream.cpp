/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "support/raw_socket_istream.h"
#include "tcpsockets/NetworkStream.h"

using namespace wpi;

void raw_socket_istream::read_impl(void* data, std::size_t len) {
  char* cdata = static_cast<char*>(data);
  std::size_t pos = 0;

  while (pos < len) {
    NetworkStream::Error err;
    std::size_t count =
        m_stream.receive(&cdata[pos], len - pos, &err, m_timeout);
    if (count == 0) {
      error_detected();
      return;
    }
    pos += count;
  }
}

void raw_socket_istream::close() { m_stream.close(); }

std::size_t raw_socket_istream::in_avail() const { return 0; }
