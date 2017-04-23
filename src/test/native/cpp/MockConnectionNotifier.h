/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_TEST_MOCKCONNECTIONNOTIFIER_H_
#define NT_TEST_MOCKCONNECTIONNOTIFIER_H_

#include "gmock/gmock.h"

#include "IConnectionNotifier.h"

namespace nt {

class MockConnectionNotifier : public IConnectionNotifier {
 public:
  MOCK_METHOD3(NotifyConnection,
               void(bool connected, const ConnectionInfo& conn_info,
                    unsigned int only_listener));
};

}  // namespace nt

#endif  // NT_TEST_MOCKCONNECTIONNOTIFIER_H_
