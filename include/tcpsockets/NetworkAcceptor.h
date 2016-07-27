/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_TCPSOCKETS_NETWORKACCEPTOR_H_
#define WPIUTIL_TCPSOCKETS_NETWORKACCEPTOR_H_

#include "tcpsockets/NetworkStream.h"

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

#endif  // WPIUTIL_TCPSOCKETS_NETWORKACCEPTOR_H_
