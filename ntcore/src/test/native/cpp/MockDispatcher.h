// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_MOCKDISPATCHER_H_
#define NTCORE_MOCKDISPATCHER_H_

#include <memory>

#include "IDispatcher.h"
#include "gmock/gmock.h"

namespace nt {

class MockDispatcher : public IDispatcher {
 public:
  MOCK_METHOD3(QueueOutgoing,
               void(std::shared_ptr<Message> msg, INetworkConnection* only,
                    INetworkConnection* except));
};

}  // namespace nt

#endif  // NTCORE_MOCKDISPATCHER_H_
