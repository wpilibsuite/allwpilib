/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_SUPPORT_RAW_SOCKET_ISTREAM_H_
#define WPIUTIL_SUPPORT_RAW_SOCKET_ISTREAM_H_

#include "support/raw_istream.h"

namespace wpi {

class NetworkStream;

class raw_socket_istream : public raw_istream {
 public:
  explicit raw_socket_istream(NetworkStream& stream, int timeout = 0)
      : m_stream(stream), m_timeout(timeout) {}

  void close() override;
  std::size_t in_avail() const override;

 private:
  void read_impl(void* data, std::size_t len) override;

  NetworkStream& m_stream;
  int m_timeout;
};

}  // namespace wpi

#endif  // WPIUTIL_SUPPORT_RAW_SOCKET_ISTREAM_H_
