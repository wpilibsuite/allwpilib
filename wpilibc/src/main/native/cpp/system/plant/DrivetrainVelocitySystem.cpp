/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/system/plant/DrivetrainVelocitySystem.h"

#include <cmath>

#include "frc/StateSpaceUtil.h"

namespace frc {

LinearSystem<2, 2, 2> DrivetrainVelocitySystem(
    DCMotor motor, units::kilogram_t m, units::meter_t r, units::meter_t rb,
    double G, units::kilogram_square_meter_t J) {
  auto C1 = -std::pow(G, 2) * motor.Kt /
            (motor.Kv * motor.R * units::math::pow<2>(r));
  auto C2 = G * motor.Kt / (motor.R * r);

  auto A = frc::MakeMatrix<2, 2>(
      ((1 / m + units::math::pow<2>(rb) / J) * C1).to<double>(),
      ((1 / m - units::math::pow<2>(rb) / J) * C1).to<double>(),
      ((1 / m - units::math::pow<2>(rb) / J) * C1).to<double>(),
      ((1 / m + units::math::pow<2>(rb) / J) * C1).to<double>());
  auto B = frc::MakeMatrix<2, 2>(
      ((1 / m + units::math::pow<2>(rb) / J) * C2).to<double>(),
      ((1 / m - units::math::pow<2>(rb) / J) * C2).to<double>(),
      ((1 / m - units::math::pow<2>(rb) / J) * C2).to<double>(),
      ((1 / m + units::math::pow<2>(rb) / J) * C2).to<double>());
  auto C = frc::MakeMatrix<2, 2>(1.0, 0.0, 0.0, 1.0);
  auto D = frc::MakeMatrix<2, 2>(0.0, 0.0, 0.0, 0.0);
  auto uMin = frc::MakeMatrix<2, 1>(-12.0, -12.0);
  auto uMax = frc::MakeMatrix<2, 1>(12.0, 12.0);

  return LinearSystem<2, 2, 2>(A, B, C, D, uMin, uMax);
}

}  // namespace frc
