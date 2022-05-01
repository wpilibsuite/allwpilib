// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/deprecated.h>

#include "frc/PIDSource.h"
#include "frc/Ultrasonic.h"

namespace frc {

/**
 * Wrapper so that PIDSource is implemented for Ultrasonic for old PIDController
 *
 * This class is provided by the OldCommands VendorDep
 *
 * @deprecated Use frc2::PIDController class instead which doesn't require this
 * wrapper.
 */
class WPI_DEPRECATED("Use frc2::PIDController class instead.") PIDUltrasonic
    : public PIDSource,
      public Ultrasonic {
  using Ultrasonic::Ultrasonic;

 public:
  /**
   * Get the range for the PIDSource base object.
   *
   * @return The range
   */
  double PIDGet() override;
};

}  // namespace frc
