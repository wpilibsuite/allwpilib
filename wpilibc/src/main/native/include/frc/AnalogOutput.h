// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/Types.h>

#include "frc/ErrorBase.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

class SendableBuilder;

/**
 * MXP analog output class.
 */
class AnalogOutput : public ErrorBase,
                     public Sendable,
                     public SendableHelper<AnalogOutput> {
 public:
  /**
   * Construct an analog output on the given channel.
   *
   * All analog outputs are located on the MXP port.
   *
   * @param channel The channel number on the roboRIO to represent.
   */
  explicit AnalogOutput(int channel);

  ~AnalogOutput() override;

  AnalogOutput(AnalogOutput&&) = default;
  AnalogOutput& operator=(AnalogOutput&&) = default;

  /**
   * Set the value of the analog output.
   *
   * @param voltage The output value in Volts, from 0.0 to +5.0
   */
  void SetVoltage(double voltage);

  /**
   * Get the voltage of the analog output
   *
   * @return The value in Volts, from 0.0 to +5.0
   */
  double GetVoltage() const;

  /**
   * Get the channel of this AnalogOutput.
   */
  int GetChannel() const;

  void InitSendable(SendableBuilder& builder) override;

 protected:
  int m_channel;
  hal::Handle<HAL_AnalogOutputHandle> m_port;
};

}  // namespace frc
