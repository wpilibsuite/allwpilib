// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/AnalogInput.h"
#include "wpi/hal/Types.hpp"
#include "wpi/util/sendable/Sendable.hpp"
#include "wpi/util/sendable/SendableHelper.hpp"

namespace wpi {

/**
 * Analog input class.
 *
 * Connected to each analog channel is an averaging and oversampling engine.
 * This engine accumulates the specified ( by SetAverageBits() and
 * SetOversampleBits() ) number of samples before returning a new value. This is
 * not a sliding window average. The only difference between the oversampled
 * samples and the averaged samples is that the oversampled samples are simply
 * accumulated effectively increasing the resolution, while the averaged samples
 * are divided by the number of samples to retain the resolution, but get more
 * stable values.
 */
class AnalogInput : public wpi::util::Sendable,
                    public wpi::util::SendableHelper<AnalogInput> {
 public:
  /**
   * Construct an analog input.
   *
   * @param channel The SmartIO channel to use.
   */
  explicit AnalogInput(int channel);

  AnalogInput(AnalogInput&&) = default;
  AnalogInput& operator=(AnalogInput&&) = default;

  ~AnalogInput() override = default;

  /**
   * Get a sample straight from this channel.
   *
   * The sample is a 12-bit value representing the 0V to 3.3V range of the A/D
   * converter in the module.  The units are in A/D converter codes.  Use
   * GetVoltage() to get the analog value in calibrated units.
   *
   * @return A sample straight from this channel.
   */
  int GetValue() const;

  /**
   * Get a scaled sample straight from this channel.
   *
   * The value is scaled to units of Volts.
   *
   * @return A scaled sample straight from this channel.
   */
  double GetVoltage() const;

  /**
   * Get the channel number.
   *
   * @return The channel number.
   */
  int GetChannel() const;

  /**
   * Indicates this input is used by a simulated device.
   *
   * @param device simulated device handle
   */
  void SetSimDevice(HAL_SimDeviceHandle device);

  void InitSendable(wpi::util::SendableBuilder& builder) override;

 private:
  int m_channel;
  wpi::hal::Handle<HAL_AnalogInputHandle, HAL_FreeAnalogInputPort> m_port;
};

}  // namespace wpi
