/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_SUPPORT_RAW_SOCKET_OSTREAM_H_
#define WPIUTIL_SUPPORT_RAW_SOCKET_OSTREAM_H_

#include "llvm/raw_ostream.h"

namespace wpi {

class NetworkStream;

class raw_socket_ostream : public llvm::raw_ostream {
 public:
  raw_socket_ostream(NetworkStream& stream, bool shouldClose)
      : m_stream(stream), m_shouldClose(shouldClose) {}
  ~raw_socket_ostream();

  void close();

  bool has_error() const { return m_error; }
  void clear_error() { m_error = false; }

 protected:
  void error_detected() { m_error = true; }

 private:
  void write_impl(const char* data, std::size_t len) override;
  uint64_t current_pos() const override;

  NetworkStream& m_stream;
  bool m_error = false;
  bool m_shouldClose;
};

}  // namespace wpi

#endif  // WPIUTIL_SUPPORT_RAW_SOCKET_OSTREAM_H_
