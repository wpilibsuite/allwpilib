/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SensorBase.h"

#include "WPIErrors.h"

const int frc::SensorBase::kDigitalChannels;
const int frc::SensorBase::kAnalogInputs;
const int frc::SensorBase::kSolenoidChannels;
const int frc::SensorBase::kSolenoidModules;
const int frc::SensorBase::kPwmChannels;
const int frc::SensorBase::kRelayChannels;
const int frc::SensorBase::kPDPChannels;

/**
 * Check that the solenoid module number is valid.
 *
 * @return Solenoid module number is valid
 */
bool frc::SensorBase::CheckSolenoidModule(int moduleNumber) {
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
bool frc::SensorBase::CheckDigitalChannel(int channel) {
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
bool frc::SensorBase::CheckRelayChannel(int channel) {
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
bool frc::SensorBase::CheckPWMChannel(int channel) {
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
bool frc::SensorBase::CheckAnalogInputChannel(int channel) {
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
bool frc::SensorBase::CheckAnalogOutputChannel(int channel) {
  if (channel >= 0 && channel < kAnalogOutputs) return true;
  return false;
}

/**
 * Verify that the solenoid channel number is within limits.
 *
 * @return Solenoid channel is valid
 */
bool frc::SensorBase::CheckSolenoidChannel(int channel) {
  if (channel >= 0 && channel < kSolenoidChannels) return true;
  return false;
}

/**
 * Verify that the power distribution channel number is within limits.
 *
 * @return PDP channel is valid
 */
bool frc::SensorBase::CheckPDPChannel(int channel) {
  if (channel >= 0 && channel < kPDPChannels) return true;
  return false;
}
