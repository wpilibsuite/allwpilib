/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
