// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/kinematics/ChassisSpeeds.h"

namespace frc {

constexpr ChassisSpeeds ChassisSpeeds::operator+(
    const ChassisSpeeds& other) const {
  return {vx + other.vx, vy + other.vy, omega + other.omega};
}

constexpr ChassisSpeeds ChassisSpeeds::operator-(
    const ChassisSpeeds& other) const {
  return *this + -other;
}

constexpr ChassisSpeeds ChassisSpeeds::operator-() const {
  return {-vx, -vy, -omega};
}

constexpr ChassisSpeeds ChassisSpeeds::operator*(double scalar) const {
  return {scalar * vx, scalar * vy, scalar * omega};
}

constexpr ChassisSpeeds ChassisSpeeds::operator/(double scalar) const {
  return operator*(1.0 / scalar);
}

}  // namespace frc
