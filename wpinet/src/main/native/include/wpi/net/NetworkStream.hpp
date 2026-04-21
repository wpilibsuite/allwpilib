// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <string_view>

namespace wpi::net {

class NetworkStream {
 public:
  NetworkStream() = default;
  virtual ~NetworkStream() = default;

  enum class Error {
    CONNECTION_CLOSED = 0,
    CONNECTION_RESET = -1,
    CONNECTION_TIMED_OUT = -2,
    WOULD_BLOCK = -3
  };

  virtual size_t send(const char* buffer, size_t len, Error* err) = 0;
  virtual size_t receive(char* buffer, size_t len, Error* err,
                         int timeout = 0) = 0;
  virtual void close() = 0;

  virtual std::string_view getPeerIP() const = 0;
  virtual int getPeerPort() const = 0;
  virtual void setNoDelay() = 0;

  // returns false on failure
  virtual bool setBlocking(bool enabled) = 0;
  virtual int getNativeHandle() const = 0;

  NetworkStream(const NetworkStream&) = delete;
  NetworkStream& operator=(const NetworkStream&) = delete;
};

}  // namespace wpi::net
