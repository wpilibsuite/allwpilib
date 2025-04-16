// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <hal/simulation/AddressableLEDData.h>
#include <hal/simulation/AnalogInData.h>
#include <hal/simulation/CTREPCMData.h>
#include <hal/simulation/CanData.h>
#include <hal/simulation/DIOData.h>
#include <hal/simulation/DigitalPWMData.h>
#include <hal/simulation/DriverStationData.h>
#include <hal/simulation/DutyCycleData.h>
#include <hal/simulation/EncoderData.h>
#include <hal/simulation/I2CData.h>
#include <hal/simulation/PWMData.h>
#include <hal/simulation/PowerDistributionData.h>
#include <hal/simulation/REVPHData.h>
#include <hal/simulation/RoboRioData.h>
#include <hal/simulation/SimDeviceData.h>

#include "../PortsInternal.h"

extern "C" void HALSIM_ResetAllSimData(void) {
  for (int32_t i = 0; i < hal::kNumAddressableLEDs; i++) {
    HALSIM_ResetAddressableLEDData(i);
  }

  for (int32_t i = 0; i < hal::kNumAnalogInputs; i++) {
    HALSIM_ResetAnalogInData(i);
  }

  HALSIM_ResetCanData();

  for (int32_t i = 0; i < hal::kNumCTREPCMModules; i++) {
    HALSIM_ResetCTREPCMData(i);
  }

  for (int32_t i = 0; i < hal::kNumDigitalPWMOutputs; i++) {
    HALSIM_ResetDigitalPWMData(i);
  }

  for (int32_t i = 0; i < hal::kNumDigitalChannels; i++) {
    HALSIM_ResetDIOData(i);
  }

  HALSIM_ResetDriverStationData();

  for (int32_t i = 0; i < hal::kNumDutyCycles; i++) {
    HALSIM_ResetDutyCycleData(i);
  }

  for (int32_t i = 0; i < hal::kNumEncoders; i++) {
    HALSIM_ResetEncoderData(i);
  }

  for (int32_t i = 0; i < hal::kI2CPorts; i++) {
    HALSIM_ResetI2CData(i);
  }

  for (int32_t i = 0; i < hal::kNumPDSimModules; i++) {
    HALSIM_ResetPowerDistributionData(i);
  }

  for (int32_t i = 0; i < hal::kNumPWMChannels; i++) {
    HALSIM_ResetPWMData(i);
  }

  for (int32_t i = 0; i < hal::kNumREVPHModules; i++) {
    HALSIM_ResetREVPHData(i);
  }

  HALSIM_ResetRoboRioData();
  HALSIM_ResetSimDeviceData();
}
