/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALSimWSServer.h"

#include <WSProviderContainer.h>
#include <WSProvider_Analog.h>
#include <WSProvider_DIO.h>
#include <WSProvider_DriverStation.h>
#include <WSProvider_Encoder.h>
#include <WSProvider_Joystick.h>
#include <WSProvider_PWM.h>
#include <WSProvider_Relay.h>
#include <WSProvider_RoboRIO.h>
#include <WSProvider_SimDevice.h>
#include <WSProvider_dPWM.h>

using namespace wpilibws;

bool HALSimWSServer::Initialize() {
  bool result = true;
  runner.ExecSync([&](wpi::uv::Loop& loop) {
    simWeb = std::make_shared<HALSimWeb>(loop, providers, simDevices);

    if (!simWeb->Initialize()) {
      result = false;
      return;
    }

    WSRegisterFunc registerFunc = [&](auto key, auto provider) {
      providers.Add(key, provider);
    };

    HALSimWSProviderAnalogIn::Initialize(registerFunc);
    HALSimWSProviderAnalogOut::Initialize(registerFunc);
    HALSimWSProviderDIO::Initialize(registerFunc);
    HALSimWSProviderDigitalPWM::Initialize(registerFunc);
    HALSimWSProviderDriverStation::Initialize(registerFunc);
    HALSimWSProviderEncoder::Initialize(registerFunc);
    HALSimWSProviderJoystick::Initialize(registerFunc);
    HALSimWSProviderPWM::Initialize(registerFunc);
    HALSimWSProviderRelay::Initialize(registerFunc);
    HALSimWSProviderRoboRIO::Initialize(registerFunc);

    simDevices.Initialize(loop);

    simWeb->Start();
  });

  return result;
}
