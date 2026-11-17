// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/DIO.h"
#include "wpi/telemetry/TelemetryLoggable.hpp"
#include "wpi/util/Handle.hpp"

namespace wpi {

/**
 * Class to write to digital outputs.
 *
 * Write values to the digital output channels. Other devices implemented
 * elsewhere will allocate channels automatically so for those devices it
 * shouldn't be done here.
 */
class DigitalOutput : public wpi::telemetry::TelemetryLoggable {
 public:
  /**
   * Create an instance of a digital output.
   *
   * Create a digital output given a channel.
   *
   * @param channel The SmartIO channel
   */
  explicit DigitalOutput(int channel);

  DigitalOutput(DigitalOutput&&) = default;
  DigitalOutput& operator=(DigitalOutput&&) = default;

  ~DigitalOutput() override;

  /**
   * Set the value of a digital output.
   *
   * Set the value of a digital output to either one (true) or zero (false).
   *
   * @param value 1 (true) for high, 0 (false) for disabled
   * @Common This is one of the commonly used methods for this class
   */
  void Set(bool value);

  /**
   * Gets the value being output from the Digital Output.
   *
   * @return the state of the digital output.
   */
  bool Get() const;

  /**
   * @return The GPIO channel number that this object represents.
   */
  int GetChannel() const;

  /**
   * Indicates this output is used by a simulated device.
   *
   * @param device simulated device handle
   */
  void SetSimDevice(HAL_SimDeviceHandle device);

  void LogTo(wpi::telemetry::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

 private:
  int m_channel;
  wpi::util::Handle<HAL_DigitalHandle, HAL_FreeDIOPort> m_handle;
};

}  // namespace wpi
