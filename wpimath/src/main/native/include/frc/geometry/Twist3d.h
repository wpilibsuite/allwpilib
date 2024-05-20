// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Rotation3d.h"
#include "units/angle.h"
#include "units/length.h"
#include "units/math.h"

namespace frc {
/**
 * A change in distance along a 3D arc since the last pose update. We can use
 * ideas from differential calculus to create new Pose3ds from a Twist3d and
 * vice versa.
 *
 * A Twist can be used to represent a difference between two poses.
 */
struct WPILIB_DLLEXPORT Twist3d {
  /**
   * Linear "dx" component
   */
  units::meter_t dx = 0_m;

  /**
   * Linear "dy" component
   */
  units::meter_t dy = 0_m;

  /**
   * Linear "dz" component
   */
  units::meter_t dz = 0_m;

  /**
   * Rotation vector x component.
   */
  units::radian_t rx = 0_rad;

  /**
   * Rotation vector y component.
   */
  units::radian_t ry = 0_rad;

  /**
   * Rotation vector z component.
   */
  units::radian_t rz = 0_rad;

  /**
   * Checks equality between this Twist3d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const Twist3d& other) const {
    return units::math::abs(dx - other.dx) < 1E-9_m &&
           units::math::abs(dy - other.dy) < 1E-9_m &&
           units::math::abs(dz - other.dz) < 1E-9_m &&
           units::math::abs(rx - other.rx) < 1E-9_rad &&
           units::math::abs(ry - other.ry) < 1E-9_rad &&
           units::math::abs(rz - other.rz) < 1E-9_rad;
  }

  /**
   * Scale this by a given factor.
   *
   * @param factor The factor by which to scale.
   * @return The scaled Twist3d.
   */
  constexpr Twist3d operator*(double factor) const {
    return Twist3d{dx * factor, dy * factor, dz * factor,
                   rx * factor, ry * factor, rz * factor};
  }
};
}  // namespace frc

#include "frc/geometry/proto/Twist3dProto.h"
#include "frc/geometry/struct/Twist3dStruct.h"
