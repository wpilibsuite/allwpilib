// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "wpi/hal/simulation/AddressableLEDData.h"
#include "wpi/hal/simulation/AnalogInData.h"
#include "wpi/hal/simulation/CTREPCMData.h"
#include "wpi/hal/simulation/CanData.h"
#include "wpi/hal/simulation/DIOData.h"
#include "wpi/hal/simulation/DigitalPWMData.h"
#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/hal/simulation/DutyCycleData.h"
#include "wpi/hal/simulation/EncoderData.h"
#include "wpi/hal/simulation/I2CData.h"
#include "wpi/hal/simulation/PWMData.h"
#include "wpi/hal/simulation/PowerDistributionData.h"
#include "wpi/hal/simulation/REVPHData.h"
#include "wpi/hal/simulation/RoboRioData.h"
#include "wpi/hal/simulation/SimDeviceData.h"

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
