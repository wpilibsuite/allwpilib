/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/ErrorBase.h"
#include "frc/PIDSource.h"
#include "frc/interfaces/Gyro.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

/**
 * GyroBase is the common base class for Gyro implementations such as
 * AnalogGyro.
 */
class GyroBase : public Gyro,
                 public ErrorBase,
                 public PIDSource,
                 public Sendable,
                 public SendableHelper<GyroBase> {
 public:
  GyroBase() = default;
  GyroBase(GyroBase&&) = default;
  GyroBase& operator=(GyroBase&&) = default;

  // PIDSource interface
  /**
   * Get the PIDOutput for the PIDSource base object. Can be set to return
   * angle or rate using SetPIDSourceType(). Defaults to angle.
   *
   * @return The PIDOutput (angle or rate, defaults to angle)
   */
  double PIDGet() override;

  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
