/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_MOCKCONNECTIONNOTIFIER_H_
#define NTCORE_MOCKCONNECTIONNOTIFIER_H_

#include "IConnectionNotifier.h"
#include "gmock/gmock.h"

namespace nt {

class MockConnectionNotifier : public IConnectionNotifier {
 public:
  MOCK_METHOD1(
      Add,
      unsigned int(
          std::function<void(const ConnectionNotification& event)> callback));
  MOCK_METHOD1(AddPolled, unsigned int(unsigned int poller_uid));
  MOCK_METHOD3(NotifyConnection,
               void(bool connected, const ConnectionInfo& conn_info,
                    unsigned int only_listener));
};

}  // namespace nt

#endif  // NTCORE_MOCKCONNECTIONNOTIFIER_H_
