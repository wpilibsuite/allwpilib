/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/system/plant/SingleJointedArmSystem.h"

#include <cmath>

#include "frc/StateSpaceUtil.h"

namespace frc {

LinearSystem<2, 1, 1> SingleJointedArmSystem(DCMotor motor,
                                             units::kilogram_square_meter_t J,
                                             double G) {
  auto A = frc::MakeMatrix<2, 2>(
      0.0, 1.0, 0.0,
      (-std::pow(G, 2) * motor.Kt / (motor.Kv * motor.R * J)).to<double>());
  auto B =
      frc::MakeMatrix<2, 1>(0.0, (G * motor.Kt / (motor.R * J)).to<double>());
  auto C = frc::MakeMatrix<1, 2>(1.0, 0.0);
  auto D = frc::MakeMatrix<1, 1>(0.0);
  auto uMin = frc::MakeMatrix<1, 1>(-12.0);
  auto uMax = frc::MakeMatrix<1, 1>(12.0);

  return LinearSystem<2, 1, 1>(A, B, C, D, uMin, uMax);
}

}  // namespace frc
