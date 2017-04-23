/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_TEST_MOCKDISPATCHER_H_
#define NT_TEST_MOCKDISPATCHER_H_

#include "gmock/gmock.h"

#include "IDispatcher.h"

namespace nt {

class MockDispatcher : public IDispatcher {
 public:
  MOCK_METHOD3(QueueOutgoing,
               void(std::shared_ptr<Message> msg, NetworkConnection* only,
                    NetworkConnection* except));
};

}  // namespace nt

#endif  // NT_TEST_MOCKDISPATCHER_H_
