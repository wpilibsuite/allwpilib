// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents the position of one swerve module.
 */
struct WPILIB_DLLEXPORT SwerveModulePosition {
  /**
   * Distance the wheel of a module has traveled
   */
  wpi::units::meter_t distance = 0_m;

  /**
   * Angle of the module.
   */
  Rotation2d angle;

  /**
   * Checks equality between this SwerveModulePosition and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const SwerveModulePosition& other) const {
    return wpi::units::math::abs(distance - other.distance) < 1E-9_m &&
           angle == other.angle;
  }

  constexpr SwerveModulePosition Interpolate(
      const SwerveModulePosition& endValue, double t) const {
    return {wpi::util::Lerp(distance, endValue.distance, t),
            wpi::util::Lerp(angle, endValue.angle, t)};
  }
};
}  // namespace wpi::math

#include "wpi/math/kinematics/proto/SwerveModulePositionProto.hpp"
#include "wpi/math/kinematics/struct/SwerveModulePositionStruct.hpp"
