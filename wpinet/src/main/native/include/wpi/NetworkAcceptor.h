// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_NETWORKACCEPTOR_H_
#define WPIUTIL_WPI_NETWORKACCEPTOR_H_

#include <memory>

#include "wpi/NetworkStream.h"

namespace wpi {

class NetworkAcceptor {
 public:
  NetworkAcceptor() = default;
  virtual ~NetworkAcceptor() = default;

  virtual int start() = 0;
  virtual void shutdown() = 0;
  virtual std::unique_ptr<NetworkStream> accept() = 0;

  NetworkAcceptor(const NetworkAcceptor&) = delete;
  NetworkAcceptor& operator=(const NetworkAcceptor&) = delete;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_NETWORKACCEPTOR_H_
