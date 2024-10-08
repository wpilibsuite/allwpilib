// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/AnalogOutput.h>
#include <hal/Types.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

namespace frc {

/**
 * MXP analog output class.
 */
class AnalogOutput : public wpi::Sendable,
                     public wpi::SendableHelper<AnalogOutput> {
 public:
  /**
   * Construct an analog output on the given channel.
   *
   * All analog outputs are located on the MXP port.
   *
   * @param channel The channel number on the roboRIO to represent.
   */
  explicit AnalogOutput(int channel);

  AnalogOutput(AnalogOutput&&) = default;
  AnalogOutput& operator=(AnalogOutput&&) = default;

  ~AnalogOutput() override = default;

  /**
   * Set the value of the analog output.
   *
   * @param voltage The output value in Volts, from 0.0 to +5.0.
   */
  void SetVoltage(double voltage);

  /**
   * Get the voltage of the analog output.
   *
   * @return The value in Volts, from 0.0 to +5.0.
   */
  double GetVoltage() const;

  /**
   * Get the channel of this AnalogOutput.
   */
  int GetChannel() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 protected:
  int m_channel;
  hal::Handle<HAL_AnalogOutputHandle, HAL_FreeAnalogOutputPort> m_port;
};

}  // namespace frc
