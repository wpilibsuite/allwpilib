// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/HALSimXRPClient.hpp"

#include <memory>

#include "wpi/halsim/xrp/HALSimXRPGui.hpp"
#include "wpi/net/EventLoopRunner.hpp"

using namespace wpilibxrp;

bool HALSimXRPClient::Initialize() {
  bool result = true;
  runner.ExecSync([&](wpi::net::uv::Loop& loop) {
    simxrp = std::make_shared<HALSimXRP>(loop);

    if (!simxrp->Initialize()) {
      result = false;
      return;
    }

    InitializeXRPBluetoothGui(simxrp);

    simxrp->Start();
  });

  return result;
}
