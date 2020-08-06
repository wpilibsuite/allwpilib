/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/geometry/Rotation2d.h"
#include "units/velocity.h"

namespace frc {
/**
 * Represents the state of one swerve module.
 */
struct SwerveModuleState {
  /**
   * Speed of the wheel of the module.
   */
  units::meters_per_second_t speed = 0_mps;

  /**
   * Angle of the module.
   */
  Rotation2d angle;
};
}  // namespace frc
