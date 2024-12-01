// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SystemServer.h"

#include "mrc/NtNetComm.h"

namespace hal {

static nt::NetworkTableInstance ServerInstance;

void InitializeSystemServer() {
  if (ServerInstance) {
    return;
  }
  ServerInstance = nt::NetworkTableInstance::Create();
  ServerInstance.SetServer("localhost", ROBOT_SYSTEM_SERVER_NT_PORT);
  ServerInstance.StartClient4("RobotProgram");
}

void ShutdownSystemServer() {
  if (!ServerInstance) {
    return;
  }

  ServerInstance.StopClient();
  nt::NetworkTableInstance::Destroy(ServerInstance);
}

nt::NetworkTableInstance GetSystemServer() {
  return ServerInstance;
}

}  // namespace hal
