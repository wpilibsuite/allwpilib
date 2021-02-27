// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_RAW_SOCKET_ISTREAM_H_
#define WPIUTIL_WPI_RAW_SOCKET_ISTREAM_H_

#include "wpi/raw_istream.h"

namespace wpi {

class NetworkStream;

class raw_socket_istream : public raw_istream {
 public:
  explicit raw_socket_istream(NetworkStream& stream, int timeout = 0)
      : m_stream(stream), m_timeout(timeout) {}

  void close() override;
  size_t in_avail() const override;

 private:
  void read_impl(void* data, size_t len) override;

  NetworkStream& m_stream;
  int m_timeout;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_RAW_SOCKET_ISTREAM_H_
