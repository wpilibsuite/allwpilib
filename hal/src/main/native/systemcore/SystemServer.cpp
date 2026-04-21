// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/SystemServer.h"

#include "SystemServerInternal.hpp"
#include "mrc/NtNetComm.h"

namespace wpi::hal {

static wpi::nt::NetworkTableInstance ServerInstance;

void InitializeSystemServer() {
  if (ServerInstance) {
    return;
  }
  ServerInstance = wpi::nt::NetworkTableInstance::Create();
  ServerInstance.SetServer("localhost", ROBOT_SYSTEM_SERVER_NT_PORT);
  ServerInstance.StartClient("RobotProgram");
}

void ShutdownSystemServer() {
  if (!ServerInstance) {
    return;
  }

  ServerInstance.StopClient();
  wpi::nt::NetworkTableInstance::Destroy(ServerInstance);
}

wpi::nt::NetworkTableInstance GetSystemServer() {
  return ServerInstance;
}

}  // namespace wpi::hal

extern "C" {

NT_Inst HAL_GetSystemServerHandle(void) {
  return wpi::hal::ServerInstance.GetHandle();
}
}  // extern "C"
