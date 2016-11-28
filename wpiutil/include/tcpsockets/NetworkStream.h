/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_TCPSOCKETS_NETWORKSTREAM_H_
#define WPIUTIL_TCPSOCKETS_NETWORKSTREAM_H_

#include <cstddef>

#include "llvm/StringRef.h"

namespace wpi {

class NetworkStream {
 public:
  NetworkStream() = default;
  virtual ~NetworkStream() = default;

  enum Error {
    kConnectionClosed = 0,
    kConnectionReset = -1,
    kConnectionTimedOut = -2,
    kWouldBlock = -3
  };

  virtual std::size_t send(const char* buffer, std::size_t len, Error* err) = 0;
  virtual std::size_t receive(char* buffer, std::size_t len, Error* err,
                              int timeout = 0) = 0;
  virtual void close() = 0;

  virtual llvm::StringRef getPeerIP() const = 0;
  virtual int getPeerPort() const = 0;
  virtual void setNoDelay() = 0;

  // returns false on failure
  virtual bool setBlocking(bool enabled) = 0;
  virtual int getNativeHandle() const = 0;

  NetworkStream(const NetworkStream&) = delete;
  NetworkStream& operator=(const NetworkStream&) = delete;
};

}  // namespace wpi

#endif  // WPIUTIL_TCPSOCKETS_NETWORKSTREAM_H_
