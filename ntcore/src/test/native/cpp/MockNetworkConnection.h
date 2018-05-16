/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_MOCKNETWORKCONNECTION_H_
#define NTCORE_MOCKNETWORKCONNECTION_H_

#include <memory>

#include "INetworkConnection.h"
#include "gmock/gmock.h"

namespace nt {

class MockNetworkConnection : public INetworkConnection {
 public:
  MOCK_CONST_METHOD0(info, ConnectionInfo());

  MOCK_METHOD1(QueueOutgoing, void(std::shared_ptr<Message> msg));
  MOCK_METHOD1(PostOutgoing, void(bool keep_alive));

  MOCK_CONST_METHOD0(proto_rev, unsigned int());
  MOCK_METHOD1(set_proto_rev, void(unsigned int proto_rev));

  MOCK_CONST_METHOD0(state, State());
  MOCK_METHOD1(set_state, void(State state));
};

}  // namespace nt

#endif  // NTCORE_MOCKNETWORKCONNECTION_H_
