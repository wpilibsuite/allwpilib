/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Base.h"
#include "ErrorBase.h"

namespace frc {

/**
 * Base class for all sensors.
 * Stores most recent status information as well as containing utility functions
 * for checking channels and error processing.
 */
class SensorBase : public ErrorBase {
 public:
  SensorBase() = default;
  virtual ~SensorBase() = default;

  SensorBase(const SensorBase&) = delete;
  SensorBase& operator=(const SensorBase&) = delete;

  static int GetDefaultSolenoidModule() { return 0; }

  static bool CheckSolenoidModule(int moduleNumber);
  static bool CheckDigitalChannel(int channel);
  static bool CheckRelayChannel(int channel);
  static bool CheckPWMChannel(int channel);
  static bool CheckAnalogInputChannel(int channel);
  static bool CheckAnalogOutputChannel(int channel);
  static bool CheckSolenoidChannel(int channel);
  static bool CheckPDPChannel(int channel);

  static const int kDigitalChannels;
  static const int kAnalogInputs;
  static const int kAnalogOutputs;
  static const int kSolenoidChannels;
  static const int kSolenoidModules;
  static const int kPwmChannels;
  static const int kRelayChannels;
  static const int kPDPChannels;
};

}  // namespace frc
