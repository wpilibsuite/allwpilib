/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

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

  static const int kDigitalChannels = 26;
  static const int kAnalogInputs = 8;
  static const int kAnalogOutputs = 2;
  static const int kSolenoidChannels = 8;
  static const int kSolenoidModules = 63;
  static const int kPwmChannels = 20;
  static const int kRelayChannels = 8;
  static const int kPDPChannels = 16;
};

}  // namespace frc
