/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
#include <hal/Main.h>
#include <wpi/raw_ostream.h>

#include "HALSimWSClient.h"

using namespace wpilibws;

static ProviderContainer providers;
static HALSimWSProviderSimDevices simDevices(providers);

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif

    int HALSIM_InitExtension(void) {
  wpi::outs() << "HALSim WS Client Extension Initializing\n";

  auto hws = std::make_shared<HALSimWS>(providers);
  HALSimWS::SetInstance(hws);

  if (!hws->Initialize()) {
    return -1;
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

  simDevices.Initialize();

  HAL_SetMain(nullptr, HALSimWS::Main, HALSimWS::Exit);

  wpi::outs() << "HALSim WS Client Extension Initialized\n";
  return 0;
}

}  // extern "C"
