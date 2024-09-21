// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/SwerveModuleState.h"

#include "units/math.h"

using namespace frc;

bool SwerveModuleState::operator==(const SwerveModuleState& other) const {
  return units::math::abs(speed - other.speed) < 1E-9_mps &&
         angle == other.angle;
}

void SwerveModuleState::Optimize(const Rotation2d& currentAngle) {
  auto delta = angle - currentAngle;
  if (units::math::abs(delta.Degrees()) > 90_deg) {
    speed *= -1;
    angle = angle + Rotation2d{180_deg};
  }
}

SwerveModuleState SwerveModuleState::Optimize(
    const SwerveModuleState& desiredState, const Rotation2d& currentAngle) {
  auto delta = desiredState.angle - currentAngle;
  if (units::math::abs(delta.Degrees()) > 90_deg) {
    return {-desiredState.speed, desiredState.angle + Rotation2d{180_deg}};
  } else {
    return {desiredState.speed, desiredState.angle};
  }
}
