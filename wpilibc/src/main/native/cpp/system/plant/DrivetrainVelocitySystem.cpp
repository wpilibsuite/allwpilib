/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/system/plant/DrivetrainVelocitySystem.h"

#include <cmath>

namespace frc {

LinearSystem<2, 2, 2> DrivetrainVelocitySystem(
    DCMotor motor, units::kilogram_t m, units::meter_t r, units::meter_t rb,
    double G, units::kilogram_square_meter_t J) {
  auto C1 = -std::pow(G, 2) * motor.Kt /
            (motor.Kv * motor.R * units::math::pow<2>(r));
  auto C2 = G * motor.Kt / (motor.R * r);

  Eigen::Matrix<double, 2, 2> A;
  A << ((1 / m + units::math::pow<2>(rb) / J) * C1).to<double>(),
      ((1 / m - units::math::pow<2>(rb) / J) * C1).to<double>(),
      ((1 / m - units::math::pow<2>(rb) / J) * C1).to<double>(),
      ((1 / m + units::math::pow<2>(rb) / J) * C1).to<double>();
  Eigen::Matrix<double, 2, 2> B;
  B << ((1 / m + units::math::pow<2>(rb) / J) * C2).to<double>(),
      ((1 / m - units::math::pow<2>(rb) / J) * C2).to<double>(),
      ((1 / m - units::math::pow<2>(rb) / J) * C2).to<double>(),
      ((1 / m + units::math::pow<2>(rb) / J) * C2).to<double>();
  Eigen::Matrix<double, 2, 2> C;
  C << 1, 0, 0, 1;
  Eigen::Matrix<double, 2, 2> D;
  D << 0, 0, 0, 0;
  Eigen::Matrix<double, 2, 1> uMin;
  uMin << -12.0, -12.0;
  Eigen::Matrix<double, 2, 1> uMax;
  uMax << 12.0, 12.0;
  return LinearSystem<2, 2, 2>(A, B, C, D, uMin, uMax);
}

}  // namespace frc
