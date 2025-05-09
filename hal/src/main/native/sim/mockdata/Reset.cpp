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
  for (int32_t i = 0; i < hal::NUM_ADDRESSABLE_LEDS; i++) {
    HALSIM_ResetAddressableLEDData(i);
  }

  for (int32_t i = 0; i < hal::NUM_ANALOG_INPUTS; i++) {
    HALSIM_ResetAnalogInData(i);
  }

  HALSIM_ResetCanData();

  for (int32_t i = 0; i < hal::NUM_CTRE_PCM_MODULES; i++) {
    HALSIM_ResetCTREPCMData(i);
  }

  for (int32_t i = 0; i < hal::NUM_DIGITAL_PWM_OUTPUTS; i++) {
    HALSIM_ResetDigitalPWMData(i);
  }

  for (int32_t i = 0; i < hal::NUM_DIGITAL_CHANNELS; i++) {
    HALSIM_ResetDIOData(i);
  }

  HALSIM_ResetDriverStationData();

  for (int32_t i = 0; i < hal::NUM_DUTY_CYCLES; i++) {
    HALSIM_ResetDutyCycleData(i);
  }

  for (int32_t i = 0; i < hal::NUM_ENCODERS; i++) {
    HALSIM_ResetEncoderData(i);
  }

  for (int32_t i = 0; i < hal::I2C_PORTS; i++) {
    HALSIM_ResetI2CData(i);
  }

  for (int32_t i = 0; i < hal::NUM_PD_SIM_MODULES; i++) {
    HALSIM_ResetPowerDistributionData(i);
  }

  for (int32_t i = 0; i < hal::NUM_PWM_CHANNELS; i++) {
    HALSIM_ResetPWMData(i);
  }

  for (int32_t i = 0; i < hal::NUM_REV_PH_MODULES; i++) {
    HALSIM_ResetREVPHData(i);
  }

  HALSIM_ResetRoboRioData();
  HALSIM_ResetSimDeviceData();
}
