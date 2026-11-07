// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.hpp"
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
#include "wpi/util/Alert.h"

extern "C" void HALSIM_ResetAllSimData(void) {
  for (int32_t i = 0; i < wpi::hal::NUM_ADDRESSABLE_LE_DS; i++) {
    HALSIM_ResetAddressableLEDData(i);
  }

  for (int32_t i = 0; i < wpi::hal::NUM_ANALOG_INPUTS; i++) {
    HALSIM_ResetAnalogInData(i);
  }

  HALSIM_ResetCanData();

  for (int32_t i = 0; i < wpi::hal::NUM_CTREPCM_MODULES; i++) {
    HALSIM_ResetCTREPCMData(i);
  }

  for (int32_t i = 0; i < wpi::hal::NUM_DIGITAL_PWM_OUTPUTS; i++) {
    HALSIM_ResetDigitalPWMData(i);
  }

  for (int32_t i = 0; i < wpi::hal::NUM_DIGITAL_CHANNELS; i++) {
    HALSIM_ResetDIOData(i);
  }

  HALSIM_ResetDriverStationData();

  for (int32_t i = 0; i < wpi::hal::NUM_DUTY_CYCLES; i++) {
    HALSIM_ResetDutyCycleData(i);
  }

  for (int32_t i = 0; i < wpi::hal::NUM_ENCODERS; i++) {
    HALSIM_ResetEncoderData(i);
  }

  for (int32_t i = 0; i < wpi::hal::I2C_PORTS; i++) {
    HALSIM_ResetI2CData(i);
  }

  for (int32_t i = 0; i < wpi::hal::NUM_PD_SIM_MODULES; i++) {
    HALSIM_ResetPowerDistributionData(i);
  }

  for (int32_t i = 0; i < wpi::hal::NUM_PWM_CHANNELS; i++) {
    HALSIM_ResetPWMData(i);
  }

  for (int32_t i = 0; i < wpi::hal::NUM_REVPH_MODULES; i++) {
    HALSIM_ResetREVPHData(i);
  }

  HALSIM_ResetRoboRioData();
  HALSIM_ResetSimDeviceData();
  WPI_ResetAlertData();
}
