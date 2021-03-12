// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/ErrorBase.h"
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
                 public Sendable,
                 public SendableHelper<GyroBase> {
 public:
  GyroBase() = default;
  GyroBase(GyroBase&&) = default;
  GyroBase& operator=(GyroBase&&) = default;

  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
