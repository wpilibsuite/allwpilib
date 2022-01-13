// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/AnalogGyro.h"
#include "frc/PIDSource.h"

namespace frc {

/**
 * Wrapper so that PIDSource is implemented for AnalogGyro for old PIDController
 *
 * This class is provided by the OldCommands VendorDep
 *
 * @deprecated Use frc2::PIDController class instead which doesn't require this
 * wrapper.
 */
class WPI_DEPRECATED("Use frc2::PIDController class instead.") PIDAnalogGyro
    : public PIDSource,
      public AnalogGyro {
  using AnalogGyro::AnalogGyro;

 public:
  /**
   * Get the PIDOutput for the PIDSource base object. Can be set to return
   * angle or rate using SetPIDSourceType(). Defaults to angle.
   *
   * @return The PIDOutput (angle or rate, defaults to angle)
   */
  double PIDGet() override;
};

}  // namespace frc
