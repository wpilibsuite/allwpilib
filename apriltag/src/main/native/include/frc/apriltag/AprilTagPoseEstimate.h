// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Transform3d.h"

namespace frc {

/** A pair of AprilTag pose estimates. */
struct WPILIB_DLLEXPORT AprilTagPoseEstimate {
  /** Pose 1. */
  Transform3d pose1;

  /** Pose 2. */
  Transform3d pose2;

  /** Object-space error of pose 1. */
  double error1;

  /** Object-space error of pose 2. */
  double error2;

  /**
   * Gets the ratio of pose reprojection errors, called ambiguity. Numbers
   * above 0.2 are likely to be ambiguous.
   *
   * @return The ratio of pose reprojection errors.
   */
  double GetAmbiguity() const;
};

}  // namespace frc
