// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALSimXRPClient.h"

#include <memory>

#include <WSProviderContainer.h>
#include <WSProvider_Analog.h>
#include <WSProvider_BuiltInAccelerometer.h>
#include <WSProvider_DIO.h>
#include <WSProvider_DriverStation.h>
#include <WSProvider_Encoder.h>
#include <WSProvider_HAL.h>
#include <WSProvider_SimDevice.h>
#include <wpinet/EventLoopRunner.h>

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
    HALSimWSProviderBuiltInAccelerometer::Initialize(registerFunc);
    HALSimWSProviderDIO::Initialize(registerFunc);
    HALSimWSProviderDriverStation::Initialize(registerFunc);
    HALSimWSProviderEncoder::Initialize(registerFunc);
    HALSimWSProviderHAL::Initialize(registerFunc);

    simDevices.Initialize(loop);

    simxrp->Start();
  });

  return result;
}
