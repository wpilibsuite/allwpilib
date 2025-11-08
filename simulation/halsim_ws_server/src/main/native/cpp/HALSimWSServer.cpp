// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/ws_server/HALSimWSServer.hpp"

#include <memory>

#include "wpi/halsim/ws_core/WSProviderContainer.hpp"
#include "wpi/halsim/ws_core/WSProvider_AddressableLED.hpp"
#include "wpi/halsim/ws_core/WSProvider_Analog.hpp"
#include "wpi/halsim/ws_core/WSProvider_DIO.hpp"
#include "wpi/halsim/ws_core/WSProvider_DriverStation.hpp"
#include "wpi/halsim/ws_core/WSProvider_Encoder.hpp"
#include "wpi/halsim/ws_core/WSProvider_Joystick.hpp"
#include "wpi/halsim/ws_core/WSProvider_PCM.hpp"
#include "wpi/halsim/ws_core/WSProvider_PWM.hpp"
#include "wpi/halsim/ws_core/WSProvider_RoboRIO.hpp"
#include "wpi/halsim/ws_core/WSProvider_SimDevice.hpp"
#include "wpi/halsim/ws_core/WSProvider_Solenoid.hpp"
#include "wpi/halsim/ws_core/WSProvider_dPWM.hpp"

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

    HALSimWSProviderAddressableLED::Initialize(registerFunc);
    HALSimWSProviderAnalogIn::Initialize(registerFunc);
    HALSimWSProviderDIO::Initialize(registerFunc);
    HALSimWSProviderDigitalPWM::Initialize(registerFunc);
    HALSimWSProviderDriverStation::Initialize(registerFunc);
    HALSimWSProviderEncoder::Initialize(registerFunc);
    HALSimWSProviderJoystick::Initialize(registerFunc);
    HALSimWSProviderPCM::Initialize(registerFunc);
    HALSimWSProviderPWM::Initialize(registerFunc);
    HALSimWSProviderRoboRIO::Initialize(registerFunc);
    HALSimWSProviderSolenoid::Initialize(registerFunc);

    simDevices.Initialize(loop);

    simWeb->Start();
  });

  return result;
}
