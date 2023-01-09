// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/SwerveModulePosition.h"

#include "frc/kinematics/SwerveModuleState.h"
#include "units/math.h"

using namespace frc;

bool SwerveModulePosition::operator==(const SwerveModulePosition& other) const {
  return units::math::abs(distance - other.distance) < 1E-9_m &&
         angle == other.angle;
}
