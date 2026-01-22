// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/PWMOutput.h"
#include "wpi/hal/Types.h"
#include "wpi/units/time.hpp"
#include "wpi/util/sendable/Sendable.hpp"
#include "wpi/util/sendable/SendableHelper.hpp"

namespace wpi {
class AddressableLED;

/**
 * Class implements the PWMOutput generation in the FPGA.
 *
 * The values supplied as arguments for PWMOutput outputs range from -1.0 to 1.0. They
 * are mapped to the microseconds to keep the pulse high, with a range of 0
 * (off) to 4096. Changes are immediately sent to the FPGA, and the update
 * occurs at the next FPGA cycle (5.05ms). There is no delay.
 */
class PWMOutput : public wpi::util::Sendable, public wpi::util::SendableHelper<PWMOutput> {
 public:
  friend class AddressableLED;
  /**
   * Represents the output period in microseconds.
   */
  enum OutputPeriod {
    /**
     * PWMOutput pulses occur every 5 ms
     */
    kOutputPeriod_5Ms = 1,
    /**
     * PWMOutput pulses occur every 10 ms
     */
    kOutputPeriod_10Ms = 2,
    /**
     * PWMOutput pulses occur every 20 ms
     */
    kOutputPeriod_20Ms = 4
  };

  /**
   * Allocate a PWMOutput given a channel number.
   *
   * Checks channel value range and allocates the appropriate channel.
   * The allocation is only done to help users ensure that they don't double
   * assign channels.
   *
   * @param channel The PWMOutput channel number. 0-9 are on-board, 10-19 are on the
   *                MXP port
   * @param registerSendable If true, adds this instance to SendableRegistry
   */
  explicit PWMOutput(int channel, bool registerSendable = true);

  PWMOutput(PWMOutput&&) = default;
  PWMOutput& operator=(PWMOutput&&) = default;

  /**
   * Free the PWMOutput channel.
   *
   * Free the resource associated with the PWMOutput channel and set the value to 0.
   */
  ~PWMOutput() override;

  /**
   * Set the PWMOutput pulse time directly to the hardware.
   *
   * Write a microsecond value to a PWMOutput channel.
   *
   * @param time Microsecond PWMOutput value.
   */
  void SetPulseTime(wpi::units::microsecond_t time);

  /**
   * Get the PWMOutput pulse time directly from the hardware.
   *
   * Read a microsecond value from a PWMOutput channel.
   *
   * @return Microsecond PWMOutput control value.
   */
  wpi::units::microsecond_t GetPulseTime() const;

  /**
   * Temporarily disables the PWMOutput output. The next set call will re-enable
   * the output.
   */
  void SetDisabled();

  /**
   * Sets the PWMOutput output period.
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

 protected:
  void InitSendable(wpi::util::SendableBuilder& builder) override;

 private:
  int m_channel;
  wpi::hal::Handle<HAL_DigitalHandle, HAL_FreePWMPort> m_handle;
};

}  // namespace wpi
