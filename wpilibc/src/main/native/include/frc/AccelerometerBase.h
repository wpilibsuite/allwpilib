/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/interfaces/Accelerometer.h"
#include "frc/smartdashboard/SendableBase.h"

namespace frc {

/**
 * Interface for 3-axis accelerometers.
 */
class AccelerometerBase : public Accelerometer, public SendableBase {
 public:
  AccelerometerBase() = default;
  virtual ~AccelerometerBase() = default;

  AccelerometerBase(AccelerometerBase&&) = default;
  AccelerometerBase& operator=(AccelerometerBase&&) = default;

  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
