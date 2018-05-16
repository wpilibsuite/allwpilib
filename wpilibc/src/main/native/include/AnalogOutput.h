/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <HAL/AnalogOutput.h>

#include "ErrorBase.h"
#include "SmartDashboard/SendableBase.h"

namespace frc {

/**
 * MXP analog output class.
 */
class AnalogOutput : public ErrorBase, public SendableBase {
 public:
  explicit AnalogOutput(int channel);
  ~AnalogOutput() override;

  void SetVoltage(double voltage);
  double GetVoltage() const;
  int GetChannel();

  void InitSendable(SendableBuilder& builder) override;

 protected:
  int m_channel;
  HAL_AnalogOutputHandle m_port;
};

}  // namespace frc
