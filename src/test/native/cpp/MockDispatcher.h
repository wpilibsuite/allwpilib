/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
