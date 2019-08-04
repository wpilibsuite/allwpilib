/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/Types.h>

#include "frc/ErrorBase.h"
#include "frc/smartdashboard/SendableBase.h"

namespace frc {

/**
 * MXP analog output class.
 */
class AnalogOutput : public ErrorBase, public SendableBase {
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

  AnalogOutput(AnalogOutput&& rhs);
  AnalogOutput& operator=(AnalogOutput&& rhs);

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
  int GetChannel();

  void InitSendable(SendableBuilder& builder) override;

 protected:
  int m_channel;
  HAL_AnalogOutputHandle m_port = HAL_kInvalidHandle;
};

}  // namespace frc
