// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
