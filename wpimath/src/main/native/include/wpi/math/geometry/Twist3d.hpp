// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/util/SymbolExports.hpp"

#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"

namespace frc {

class Transform3d;

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
   * Obtain a new Transform3d from a (constant curvature) velocity.
   *
   * See "https://file.tavsys.net/control/controls-engineering-in-frc.pdf"
   * Controls Engineering in the FIRST Robotics Competition section 10.2
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
  constexpr Transform3d Exp() const;

  /**
   * Checks equality between this Twist3d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const Twist3d& other) const {
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

#include "wpi/math/geometry/Transform3d.hpp"
#include "wpi/math/geometry/detail/RotationVectorToMatrix.hpp"

namespace frc {

constexpr Transform3d Twist3d::Exp() const {
  // Implementation from Section 3.2 of https://ethaneade.org/lie.pdf

  auto impl = [this]<typename Matrix3d, typename Vector3d>() -> Transform3d {
    Vector3d u{{dx.value(), dy.value(), dz.value()}};
    Vector3d rvec{{rx.value(), ry.value(), rz.value()}};
    Matrix3d omega = detail::RotationVectorToMatrix(rvec);
    Matrix3d omegaSq = omega * omega;
    double theta = rvec.norm();
    double thetaSq = theta * theta;

    double A;
    double B;
    double C;
    if (gcem::abs(theta) < 1E-7) {
      // Taylor Expansions around θ = 0
      // A = 1/1! - θ²/3! + θ⁴/5!
      // B = 1/2! - θ²/4! + θ⁴/6!
      // C = 1/3! - θ²/5! + θ⁴/7!
      // sources:
      // A:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D+at+x%3D0
      // B:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-cos%5C%2840%29x%5C%2841%29%2CPower%5Bx%2C2%5D%5D+at+x%3D0
      // C:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-Divide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D%2CPower%5Bx%2C2%5D%5D+at+x%3D0
      A = 1 - thetaSq / 6 + thetaSq * thetaSq / 120;
      B = 1 / 2.0 - thetaSq / 24 + thetaSq * thetaSq / 720;
      C = 1 / 6.0 - thetaSq / 120 + thetaSq * thetaSq / 5040;
    } else {
      // A = std::sin(θ)/θ
      // B = (1 - std::cos(θ)) / θ²
      // C = (1 - A) / θ²
      A = gcem::sin(theta) / theta;
      B = (1 - gcem::cos(theta)) / thetaSq;
      C = (1 - A) / thetaSq;
    }

    Matrix3d R = Matrix3d::Identity() + A * omega + B * omegaSq;
    Matrix3d V = Matrix3d::Identity() + B * omega + C * omegaSq;

    Vector3d translation_component = V * u;

    const Transform3d transform{
        Translation3d{units::meter_t{translation_component(0)},
                      units::meter_t{translation_component(1)},
                      units::meter_t{translation_component(2)}},
        Rotation3d{R}};

    return transform;
  };

  if (std::is_constant_evaluated()) {
    return impl.template operator()<ct_matrix3d, ct_vector3d>();
  }
  return impl.template operator()<Eigen::Matrix3d, Eigen::Vector3d>();
}

}  // namespace frc

#include "wpi/math/geometry/proto/Twist3dProto.hpp"
#include "wpi/math/geometry/struct/Twist3dStruct.hpp"
