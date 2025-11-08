// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_RAW_SOCKET_ISTREAM_HPP_
#define WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_RAW_SOCKET_ISTREAM_HPP_

#include "wpi/util/raw_istream.hpp"

namespace wpi::net {

class NetworkStream;

class raw_socket_istream : public wpi::util::raw_istream {
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

}  // namespace wpi::net

#endif  // WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_RAW_SOCKET_ISTREAM_HPP_
