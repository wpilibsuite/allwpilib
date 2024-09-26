// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALSimWSClient.h"

#include <memory>

#include <WSProviderContainer.h>
#include <WSProvider_AddressableLED.h>
#include <WSProvider_Analog.h>
#include <WSProvider_BuiltInAccelerometer.h>
#include <WSProvider_DIO.h>
#include <WSProvider_DriverStation.h>
#include <WSProvider_Encoder.h>
#include <WSProvider_Joystick.h>
#include <WSProvider_PCM.h>
#include <WSProvider_PWM.h>
#include <WSProvider_Relay.h>
#include <WSProvider_RoboRIO.h>
#include <WSProvider_SimDevice.h>
#include <WSProvider_Solenoid.h>
#include <WSProvider_dPWM.h>
#include <wpinet/EventLoopRunner.h>

using namespace wpilibws;

bool HALSimWSClient::Initialize() {
  bool result = true;
  runner.ExecSync([&](wpi::uv::Loop& loop) {
    simws = std::make_shared<HALSimWS>(loop, providers, simDevices);

    if (!simws->Initialize()) {
      result = false;
      return;
    }

    WSRegisterFunc registerFunc = [&](auto key, auto provider) {
      providers.Add(key, provider);
    };

    HALSimWSProviderAddressableLED::Initialize(registerFunc);
    HALSimWSProviderAnalogIn::Initialize(registerFunc);
    HALSimWSProviderAnalogOut::Initialize(registerFunc);
    HALSimWSProviderBuiltInAccelerometer::Initialize(registerFunc);
    HALSimWSProviderDIO::Initialize(registerFunc);
    HALSimWSProviderDigitalPWM::Initialize(registerFunc);
    HALSimWSProviderDriverStation::Initialize(registerFunc);
    HALSimWSProviderEncoder::Initialize(registerFunc);
    HALSimWSProviderJoystick::Initialize(registerFunc);
    HALSimWSProviderPCM::Initialize(registerFunc);
    HALSimWSProviderPWM::Initialize(registerFunc);
    HALSimWSProviderRelay::Initialize(registerFunc);
    HALSimWSProviderRoboRIO::Initialize(registerFunc);
    HALSimWSProviderSolenoid::Initialize(registerFunc);

    simDevices.Initialize(loop);

    simws->Start();
  });

  return result;
}
