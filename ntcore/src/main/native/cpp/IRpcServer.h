// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_IRPCSERVER_H_
#define NTCORE_IRPCSERVER_H_

#include <memory>

#include "Message.h"
#include "ntcore_cpp.h"

namespace nt {

class IRpcServer {
 public:
  typedef std::function<void(StringRef result)> SendResponseFunc;

  IRpcServer() = default;
  IRpcServer(const IRpcServer&) = delete;
  IRpcServer& operator=(const IRpcServer&) = delete;
  virtual ~IRpcServer() = default;

  virtual void RemoveRpc(unsigned int rpc_uid) = 0;

  virtual void ProcessRpc(unsigned int local_id, unsigned int call_uid,
                          StringRef name, StringRef params,
                          const ConnectionInfo& conn,
                          SendResponseFunc send_response,
                          unsigned int rpc_uid) = 0;
};

}  // namespace nt

#endif  // NTCORE_IRPCSERVER_H_
