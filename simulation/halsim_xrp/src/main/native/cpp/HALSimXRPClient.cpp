// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/HALSimXRPClient.hpp"

#include <memory>

#include "wpi/halsim/ws_core/WSProviderContainer.hpp"
#include "wpi/halsim/ws_core/WSProvider_Analog.hpp"
#include "wpi/halsim/ws_core/WSProvider_DIO.hpp"
#include "wpi/halsim/ws_core/WSProvider_DriverStation.hpp"
#include "wpi/halsim/ws_core/WSProvider_Encoder.hpp"
#include "wpi/halsim/ws_core/WSProvider_HAL.hpp"
#include "wpi/halsim/ws_core/WSProvider_SimDevice.hpp"
#include "wpi/net/EventLoopRunner.hpp"

using namespace wpilibxrp;
using namespace wpilibws;

bool HALSimXRPClient::Initialize() {
  bool result = true;
  runner.ExecSync([&](wpi::uv::Loop& loop) {
    simxrp = std::make_shared<HALSimXRP>(loop, providers, simDevices);

    if (!simxrp->Initialize()) {
      result = false;
      return;
    }

    WSRegisterFunc registerFunc = [&](auto key, auto provider) {
      providers.Add(key, provider);
    };

    // Minimized set of HAL providers
    HALSimWSProviderAnalogIn::Initialize(registerFunc);
    HALSimWSProviderDIO::Initialize(registerFunc);
    HALSimWSProviderDriverStation::Initialize(registerFunc);
    HALSimWSProviderEncoder::Initialize(registerFunc);
    HALSimWSProviderHAL::Initialize(registerFunc);

    simDevices.Initialize(loop);

    simxrp->Start();
  });

  return result;
}
