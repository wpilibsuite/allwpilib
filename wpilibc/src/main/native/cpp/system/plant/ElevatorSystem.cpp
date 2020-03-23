/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/system/plant/ElevatorSystem.h"

#include <cmath>

#include "frc/StateSpaceUtil.h"

namespace frc {

LinearSystem<2, 1, 1> ElevatorSystem(DCMotor motor, units::kilogram_t m,
                                     units::meter_t r, double G) {
  auto A =
      frc::MakeMatrix<2, 2>(0.0, 1.0, 0.0,
                            (-std::pow(G, 2) * motor.Kt /
                             (motor.R * units::math::pow<2>(r) * m * motor.Kv))
                                .to<double>());
  auto B = frc::MakeMatrix<2, 1>(
      0.0, (G * motor.Kt / (motor.R * r * m)).to<double>());
  auto C = frc::MakeMatrix<1, 2>(1.0, 0.0);
  auto D = frc::MakeMatrix<1, 1>(0.0);
  auto uMin = frc::MakeMatrix<1, 1>(-12.0);
  auto uMax = frc::MakeMatrix<1, 1>(12.0);

  return LinearSystem<2, 1, 1>(A, B, C, D, uMin, uMax);
}

}  // namespace frc
