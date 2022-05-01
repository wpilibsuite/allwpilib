// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/deprecated.h>

#include "frc/Encoder.h"
#include "frc/PIDSource.h"

namespace frc {

/**
 * Wrapper so that PIDSource is implemented for Encoder for old PIDController
 *
 * This class is provided by the OldCommands VendorDep
 *
 * @deprecated Use frc2::PIDController class instead which doesn't require this
 * wrapper.
 */
class WPI_DEPRECATED("Use frc2::PIDController class instead.") PIDEncoder
    : public PIDSource,
      public Encoder {
  using Encoder::Encoder;

 public:
  /**
   * Get the PIDOutput for the PIDSource base object. Can be set to return
   * distance or rate using SetPIDSourceType(). Defaults to distance.
   *
   * @return The PIDOutput (distance or rate, defaults to distance)
   */
  double PIDGet() override;
};

}  // namespace frc
