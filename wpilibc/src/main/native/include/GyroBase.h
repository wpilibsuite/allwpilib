/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "ErrorBase.h"
#include "PIDSource.h"
#include "SmartDashboard/SendableBase.h"
#include "interfaces/Gyro.h"

namespace frc {

/**
 * GyroBase is the common base class for Gyro implementations such as
 * AnalogGyro.
 */
class GyroBase : public Gyro,
                 public ErrorBase,
                 public SendableBase,
                 public PIDSource {
 public:
  // PIDSource interface
  double PIDGet() override;

  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
