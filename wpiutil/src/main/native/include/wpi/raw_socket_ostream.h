// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_RAW_SOCKET_OSTREAM_H_
#define WPIUTIL_WPI_RAW_SOCKET_OSTREAM_H_

#include "wpi/raw_ostream.h"

namespace wpi {

class NetworkStream;

class raw_socket_ostream : public raw_ostream {
 public:
  raw_socket_ostream(NetworkStream& stream, bool shouldClose)
      : m_stream(stream), m_shouldClose(shouldClose) {}
  ~raw_socket_ostream() override;

  void close();

  bool has_error() const { return m_error; }
  void clear_error() { m_error = false; }

 protected:
  void error_detected() { m_error = true; }

 private:
  void write_impl(const char* data, size_t len) override;
  uint64_t current_pos() const override;

  NetworkStream& m_stream;
  bool m_error = false;
  bool m_shouldClose;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_RAW_SOCKET_OSTREAM_H_
