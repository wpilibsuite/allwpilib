/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NTCORE_MOCKRPCSERVER_H_
#define NTCORE_MOCKRPCSERVER_H_

#include "IRpcServer.h"
#include "gmock/gmock.h"

namespace nt {

class MockRpcServer : public IRpcServer {
 public:
  MOCK_METHOD0(Start, void());
  MOCK_METHOD1(RemoveRpc, void(unsigned int rpc_uid));
  MOCK_METHOD7(ProcessRpc,
               void(unsigned int local_id, unsigned int call_uid,
                    StringRef name, StringRef params,
                    const ConnectionInfo& conn, SendResponseFunc send_response,
                    unsigned int rpc_uid));
};

}  // namespace nt

#endif  // NTCORE_MOCKRPCSERVER_H_
