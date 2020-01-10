/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

#include "frc/geometry/Rotation2d.h"

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

  /**
   * Optimizes the angle of the module to make sure it doesn't 
   * rotate more than 90 degrees from the current state
   */
  SwerveModuleState OptimizeModuleAngle(const SwerveModuleState& desiredState) const {
    frc::SwerveModuleState finalStateAfterOptimization = desiredState;
    frc::Rotation2d deltaAngle{finalStateAfterOptimization.angle.Degrees() - angle.Degrees()};
    if (units::math::abs(deltaAngle.Degrees()) > 90_deg && units::math::abs(deltaAngle.Degrees()) < 270_deg) {
        units::degree_t finalAngle = units::math::fmod(finalStateAfterOptimization.angle.Degrees() + 180_deg, 360_deg);
        finalStateAfterOptimization.angle = frc::Rotation2d(finalAngle);
        finalStateAfterOptimization.speed = -finalStateAfterOptimization.speed;
    }
    return finalStateAfterOptimization;
  }
};
}  // namespace frc
