// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace frc {

class Transform2d;

/**
 * A change in distance along a 2D arc since the last pose update. We can use
 * ideas from differential calculus to create new Pose2ds from a Twist2d and
 * vice versa.
 *
 * A Twist can be used to represent a difference between two poses.
 */
struct WPILIB_DLLEXPORT Twist2d {
  /**
   * Linear "dx" component
   */
  units::meter_t dx = 0_m;

  /**
   * Linear "dy" component
   */
  units::meter_t dy = 0_m;

  /**
   * Angular "dtheta" component (radians)
   */
  units::radian_t dtheta = 0_rad;

  /**
   * Obtain a new Transform2d from a (constant curvature) velocity.
   *
   * See "https://file.tavsys.net/control/controls-engineering-in-frc.pdf"
   * Controls Engineering in the FIRST Robotics Competition</a> section 10.2
   * "Pose exponential" for a derivation.
   *
   * The twist is a change in pose in the robot's coordinate frame since the
   * previous pose update. When the user runs Exp() on the twist, the user will
   * receive the pose delta.
   *
   * "Exp" represents the pose exponential, which is solving a differential
   * equation moving the pose forward in time.
   *
   * @return The pose delta of the robot.
   */
  constexpr Transform2d Exp() const;

  /**
   * Checks equality between this Twist2d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const Twist2d& other) const {
    return units::math::abs(dx - other.dx) < 1E-9_m &&
           units::math::abs(dy - other.dy) < 1E-9_m &&
           units::math::abs(dtheta - other.dtheta) < 1E-9_rad;
  }

  /**
   * Scale this by a given factor.
   *
   * @param factor The factor by which to scale.
   * @return The scaled Twist2d.
   */
  constexpr Twist2d operator*(double factor) const {
    return Twist2d{dx * factor, dy * factor, dtheta * factor};
  }
};

}  // namespace frc

#include "wpi/math/geometry/Transform2d.hpp"

namespace frc {

constexpr Transform2d Twist2d::Exp() const {
  const auto theta = dtheta.value();
  const auto sinTheta = gcem::sin(theta);
  const auto cosTheta = gcem::cos(theta);

  double s, c;
  if (gcem::abs(theta) < 1E-9) {
    s = 1.0 - 1.0 / 6.0 * theta * theta;
    c = 0.5 * theta;
  } else {
    s = sinTheta / theta;
    c = (1 - cosTheta) / theta;
  }

  return Transform2d(Translation2d{dx * s - dy * c, dx * c + dy * s},
                     Rotation2d{cosTheta, sinTheta});
}

}  // namespace frc

#include "wpi/math/geometry/proto/Twist2dProto.hpp"
#include "wpi/math/geometry/struct/Twist2dStruct.hpp"
