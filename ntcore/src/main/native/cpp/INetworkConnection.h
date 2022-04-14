// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_INETWORKCONNECTION_H_
#define NTCORE_INETWORKCONNECTION_H_

#include <memory>

#include "Message.h"
#include "ntcore_cpp.h"

namespace nt {

class INetworkConnection {
 public:
  enum State { kCreated, kInit, kHandshake, kSynchronized, kActive, kDead };

  INetworkConnection() = default;
  INetworkConnection(const INetworkConnection&) = delete;
  INetworkConnection& operator=(const INetworkConnection&) = delete;
  virtual ~INetworkConnection() = default;

  virtual ConnectionInfo info() const = 0;

  virtual void QueueOutgoing(std::shared_ptr<Message> msg) = 0;
  virtual void PostOutgoing(bool keep_alive) = 0;

  virtual unsigned int proto_rev() const = 0;
  virtual void set_proto_rev(unsigned int proto_rev) = 0;

  virtual State state() const = 0;
  virtual void set_state(State state) = 0;
};

}  // namespace nt

#endif  // NTCORE_INETWORKCONNECTION_H_
