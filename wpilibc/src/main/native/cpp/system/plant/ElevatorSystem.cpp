/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/system/plant/ElevatorSystem.h"

#include <cmath>

namespace frc {

LinearSystem<2, 1, 1> ElevatorSystem(DCMotor motor, units::kilogram_t m,
                                     units::meter_t r, double G) {
  Eigen::Matrix<double, 2, 2> A;
  A << 0, 1, 0,
      (-std::pow(G, 2) * motor.Kt /
       (motor.R * units::math::pow<2>(r) * m * motor.Kv))
          .to<double>();
  Eigen::Matrix<double, 2, 1> B;
  B << 0, (G * motor.Kt / (motor.R * r * m)).to<double>();
  Eigen::Matrix<double, 1, 2> C;
  C << 1, 0;
  Eigen::Matrix<double, 1, 1> D;
  D << 0;
  Eigen::Matrix<double, 1, 1> uMin;
  uMin << -12.0;
  Eigen::Matrix<double, 1, 1> uMax;
  uMax << 12.0;
  return LinearSystem<2, 1, 1>(A, B, C, D, uMin, uMax);
}

}  // namespace frc
