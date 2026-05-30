// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/AntiTipping.hpp"

#include <cmath>

using namespace wpi::math;

ChassisVelocities AntiTipping::Calculate(const Rotation3d& attitude) {
  // To find the correction, we rotate the z axis (scaled by the P gain) by the
  // attitude, then project onto the x-y plane.
  Translation2d correction =
      Translation3d{0_m, 0_m, wpi::units::meter_t{m_kp}}
          .RotateBy(attitude)
          .ToTranslation2d();
  double norm = correction.Norm().value();

  // Let inclination angle of 3D correction be θ.
  //
  //    _o_       +z
  //    \  |       ^
  //   h \θ|       |
  //      \|  +x <--
  //
  // where o is length of 2D correction and h is length of 3D correction.
  //
  //   sinθ = o/h
  //   θ = asin(norm / m_kp)
  double inclinationAngle = std::asin(norm / m_kp);

  if (inclinationAngle < m_tippingThreshold) {
    return {};
  } else if (norm > m_maxCorrectionSpeed) {
    correction = correction * (m_maxCorrectionSpeed / norm);
  }

  return {wpi::units::meters_per_second_t{correction.X().value()},
          wpi::units::meters_per_second_t{correction.Y().value()},
          0_rad_per_s};
}
