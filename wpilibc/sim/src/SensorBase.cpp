/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SensorBase.h"

#include "WPIErrors.h"

using namespace frc;

const int SensorBase::kDigitalChannels;
const int SensorBase::kAnalogInputs;
const int SensorBase::kSolenoidChannels;
const int SensorBase::kSolenoidModules;
const int SensorBase::kPwmChannels;
const int SensorBase::kRelayChannels;
const int SensorBase::kPDPChannels;

/**
 * Check that the solenoid module number is valid.
 *
 * @return Solenoid module number is valid
 */
bool SensorBase::CheckSolenoidModule(int moduleNumber) {
  return moduleNumber >= 0 && moduleNumber < kSolenoidModules;
}

/**
 * Check that the digital channel number is valid.
 *
 * Verify that the channel number is one of the legal channel numbers. Channel
 * numbers are 0-based.
 *
 * @return Digital channel is valid
 */
bool SensorBase::CheckDigitalChannel(int channel) {
  if (channel >= 0 && channel < kDigitalChannels) return true;
  return false;
}

/**
 * Check that the digital channel number is valid.
 *
 * Verify that the channel number is one of the legal channel numbers. Channel
 * numbers are 0-based.
 *
 * @return Relay channel is valid
 */
bool SensorBase::CheckRelayChannel(int channel) {
  if (channel >= 0 && channel < kRelayChannels) return true;
  return false;
}

/**
 * Check that the digital channel number is valid.
 *
 * Verify that the channel number is one of the legal channel numbers. Channel
 * numbers are 0-based.
 *
 * @return PWM channel is valid
 */
bool SensorBase::CheckPWMChannel(int channel) {
  if (channel >= 0 && channel < kPwmChannels) return true;
  return false;
}

/**
 * Check that the analog input number is valid.
 *
 * Verify that the analog input number is one of the legal channel numbers.
 * Channel numbers are 0-based.
 *
 * @return Analog channel is valid
 */
bool SensorBase::CheckAnalogInputChannel(int channel) {
  if (channel >= 0 && channel < kAnalogInputs) return true;
  return false;
}

/**
 * Check that the analog output number is valid.
 *
 * Verify that the analog output number is one of the legal channel numbers.
 * Channel numbers are 0-based.
 *
 * @return Analog channel is valid
 */
bool SensorBase::CheckAnalogOutputChannel(int channel) {
  if (channel >= 0 && channel < kAnalogOutputs) return true;
  return false;
}

/**
 * Verify that the solenoid channel number is within limits.
 *
 * @return Solenoid channel is valid
 */
bool SensorBase::CheckSolenoidChannel(int channel) {
  if (channel >= 0 && channel < kSolenoidChannels) return true;
  return false;
}

/**
 * Verify that the power distribution channel number is within limits.
 *
 * @return PDP channel is valid
 */
bool SensorBase::CheckPDPChannel(int channel) {
  if (channel >= 0 && channel < kPDPChannels) return true;
  return false;
}
