/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/system/plant/FlywheelSystem.h"

#include <cmath>

namespace frc {

LinearSystem<1, 1, 1> FlywheelSystem(DCMotor motor,
                                     units::kilogram_square_meter_t J,
                                     double G) {
  Eigen::Matrix<double, 1, 1> A;
  A << (-std::pow(G, 2) * motor.Kt / (motor.Kv * motor.R * J)).to<double>();
  Eigen::Matrix<double, 1, 1> B;
  B << (G * motor.Kt / (motor.R * J)).to<double>();
  Eigen::Matrix<double, 1, 1> C;
  C << 1;
  Eigen::Matrix<double, 1, 1> D;
  D << 0;
  Eigen::Matrix<double, 1, 1> uMin;
  uMin << -12.0;
  Eigen::Matrix<double, 1, 1> uMax;
  uMax << 12.0;
  return LinearSystem<1, 1, 1>(A, B, C, D, uMin, uMax);
}

}  // namespace frc
