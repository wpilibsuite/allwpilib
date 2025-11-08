// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_NETWORKACCEPTOR_HPP_
#define WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_NETWORKACCEPTOR_HPP_

#include <memory>

#include "wpi/net/NetworkStream.hpp"

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

#endif  // WPINET_WPINET_SRC_MAIN_NATIVE_INCLUDE_WPI_NET_NETWORKACCEPTOR_HPP_
