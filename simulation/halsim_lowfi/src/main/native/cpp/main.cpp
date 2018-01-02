/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include <HALSimLowFi.h>
#include <NTProvider_Analog.h>
#include <NTProvider_DIO.h>
#include <NTProvider_DriverStation.h>
#include <NTProvider_Encoder.h>
#include <NTProvider_PWM.h>
#include <NTProvider_Relay.h>
#include <NTProvider_RoboRIO.h>
#include <NTProvider_dPWM.h>

static HALSimLowFi halsim_lowfi;

static HALSimNTProviderPWM pwm_provider;
static HALSimNTProviderDigitalPWM dpwm_provider;
static HALSimNTProviderDIO dio_provider;
static HALSimNTProviderAnalogIn ai_provider;
static HALSimNTProviderAnalogOut ao_provider;
static HALSimNTProviderDriverStation ds_provider;
static HALSimNTProviderEncoder encoder_provider;
static HALSimNTProviderRelay relay_provider;
static HALSimNTProviderRoboRIO roborio_provider;

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  std::cout << "NetworkTables LowFi Simulator Initializing." << std::endl;
  halsim_lowfi.Initialize();
  halsim_lowfi.table->GetInstance().StartServer("networktables.ini");
  halsim_lowfi.table->GetInstance().SetUpdateRate(0.05);
  auto lowfi = std::make_shared<HALSimLowFi>(halsim_lowfi);

  pwm_provider.Inject(lowfi, "PWM");
  dpwm_provider.Inject(lowfi, "dPWM");
  dio_provider.Inject(lowfi, "DIO");
  ai_provider.Inject(lowfi, "AI");
  ao_provider.Inject(lowfi, "AO");
  ds_provider.Inject(lowfi, "DriverStation");
  encoder_provider.Inject(lowfi, "Encoder");
  relay_provider.Inject(lowfi, "Relay");
  roborio_provider.Inject(lowfi, "RoboRIO");

  std::cout << "NetworkTables LowFi Simulator Initialized!" << std::endl;
  return 0;
}
}  // extern "C"
