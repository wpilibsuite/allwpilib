// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/PWM.h"
#include "wpi/hal/Types.h"
#include "wpi/telemetry/TelemetryLoggable.hpp"
#include "wpi/units/time.hpp"

namespace wpi {
class AddressableLED;

/**
 * Class implements the PWM generation in the FPGA.
 *
 * The values supplied as arguments for PWM outputs range from -1.0 to 1.0. They
 * are mapped to the microseconds to keep the pulse high, with a range of 0
 * (off) to 4096. Changes are immediately sent to the FPGA, and the update
 * occurs at the next FPGA cycle (5.05ms). There is no delay.
 */
class PWM : public wpi::TelemetryLoggable {
 public:
  friend class AddressableLED;
  /**
   * Represents the output period in microseconds.
   */
  enum OutputPeriod {
    /**
     * PWM pulses occur every 5 ms
     */
    kOutputPeriod_5Ms = 1,
    /**
     * PWM pulses occur every 10 ms
     */
    kOutputPeriod_10Ms = 2,
    /**
     * PWM pulses occur every 20 ms
     */
    kOutputPeriod_20Ms = 4
  };

  /**
   * Allocate a PWM given a channel number.
   *
   * Checks channel value range and allocates the appropriate channel.
   * The allocation is only done to help users ensure that they don't double
   * assign channels.
   *
   * @param channel The PWM channel number. 0-9 are on-board, 10-19 are on the
   *                MXP port
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
   * @param time Microsecond PWM value.
   */
  void SetPulseTime(wpi::units::microsecond_t time);

  /**
   * Get the PWM pulse time directly from the hardware.
   *
   * Read a microsecond value from a PWM channel.
   *
   * @return Microsecond PWM control value.
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
   * @param mult The output period to apply to this channel
   */
  void SetOutputPeriod(OutputPeriod mult);

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
  wpi::hal::Handle<HAL_DigitalHandle, HAL_FreePWMPort> m_handle;
};

}  // namespace wpi
