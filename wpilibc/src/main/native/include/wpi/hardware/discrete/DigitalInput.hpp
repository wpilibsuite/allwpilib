// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/DIO.h"
#include "wpi/telemetry/TelemetryLoggable.hpp"

namespace wpi {

/**
 * Class to read a digital input.
 *
 * This class will read digital inputs and return the current value on the
 * channel. Other devices such as encoders, gear tooth sensors, etc. that are
 * implemented elsewhere will automatically allocate digital inputs and outputs
 * as required. This class is only for devices like switches etc. that aren't
 * implemented anywhere else.
 */
class DigitalInput : public wpi::TelemetryLoggable {
 public:
  /**
   * Create an instance of a Digital Input class.
   *
   * Creates a digital input given a channel.
   *
   * @param channel The DIO channel 0-9 are on-board, 10-25 are on the MXP port
   */
  explicit DigitalInput(int channel);

  DigitalInput(DigitalInput&&) = default;
  DigitalInput& operator=(DigitalInput&&) = default;

  ~DigitalInput() override = default;

  /**
   * Get the value from a digital input channel.
   *
   * Retrieve the value of a single digital input channel from the FPGA.
   */
  bool Get() const;

  /**
   * @return The GPIO channel number that this object represents.
   */
  int GetChannel() const;

  /**
   * Indicates this input is used by a simulated device.
   *
   * @param device simulated device handle
   */
  void SetSimDevice(HAL_SimDeviceHandle device);

  void UpdateTelemetry(wpi::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

 private:
  int m_channel;
  wpi::hal::Handle<HAL_DigitalHandle, HAL_FreeDIOPort> m_handle;
};

}  // namespace wpi
