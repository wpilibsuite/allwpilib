// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/PWM.h"
#include "wpi/telemetry/TelemetryLoggable.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/Handle.hpp"

namespace wpi {
class AddressableLED;

/**
 * Class for sending pulse-width modulation (PWM) signals.
 */
class PWM : public wpi::TelemetryLoggable {
 public:
  friend class AddressableLED;

  /**
   * Allocate a PWM given a channel number.
   *
   * Checks channel value range and allocates the appropriate channel.
   * The allocation is only done to help users ensure that they don't double
   * assign channels.
   *
   * @param channel The SmartIO channel number.
   */
  explicit PWM(int channel);

  PWM(PWM&&) = default;
  PWM& operator=(PWM&&) = default;

  /**
   * Free the PWM channel.
   *
   * Free the resource associated with the PWM channel and set the value to 0.
   */
  ~PWM() override;

  /**
   * Set the PWM pulse time directly to the hardware.
   *
   * Write a microsecond value to a PWM channel.
   *
   * @param time Microsecond PWM value. Range 0 - 4096.
   */
  void SetPulseTime(wpi::units::microsecond_t time);

  /**
   * Get the PWM pulse time directly from the hardware.
   *
   * Read a microsecond value from a PWM channel.
   *
   * @return Microsecond PWM control value. Range 0 - 4096.
   */
  wpi::units::microsecond_t GetPulseTime() const;

  /**
   * Temporarily disables the PWM output. The next set call will re-enable
   * the output.
   */
  void SetDisabled();

  /**
   * Sets the PWM output period.
   *
   * @param period The output period to apply to this channel, in milliseconds.
   * Valid values are 5ms, 10ms, and 20ms. Default is 20 ms.
   */
  void SetOutputPeriod(wpi::units::millisecond_t period);

  int GetChannel() const;

  /**
   * Indicates this input is used by a simulated device.
   *
   * @param device simulated device handle
   */
  void SetSimDevice(HAL_SimDeviceHandle device);

  void LogTo(wpi::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

 private:
  int m_channel;
  wpi::util::Handle<HAL_DigitalHandle, HAL_FreePWMPort> m_handle;
};

}  // namespace wpi
