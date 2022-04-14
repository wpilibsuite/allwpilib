// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_IDISPATCHER_H_
#define NTCORE_IDISPATCHER_H_

#include <memory>

#include "Message.h"

namespace nt {

class INetworkConnection;

// Interface for generation of outgoing messages to break a dependency loop
// between Storage and Dispatcher.
class IDispatcher {
 public:
  IDispatcher() = default;
  IDispatcher(const IDispatcher&) = delete;
  IDispatcher& operator=(const IDispatcher&) = delete;
  virtual ~IDispatcher() = default;
  virtual void QueueOutgoing(std::shared_ptr<Message> msg,
                             INetworkConnection* only,
                             INetworkConnection* except) = 0;
};

}  // namespace nt

#endif  // NTCORE_IDISPATCHER_H_
