/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/system/plant/LinearSystemId.h"

#include "frc/StateSpaceUtil.h"

namespace frc {

LinearSystem<1, 1, 1> IdentifyVelocitySystem(double kV, double kA) {
  auto A = frc::MakeMatrix<1, 1>(-kV / kA);
  auto B = frc::MakeMatrix<1, 1>(1.0 / kA);
  auto C = frc::MakeMatrix<1, 1>(1.0);
  auto D = frc::MakeMatrix<1, 1>(0.0);
  auto uMin = frc::MakeMatrix<1, 1>(-12.0);
  auto uMax = frc::MakeMatrix<1, 1>(12.0);

  return LinearSystem<1, 1, 1>(A, B, C, D, uMin, uMax);
}

LinearSystem<2, 1, 1> IdentifyPositionSystem(double kV, double kA) {
  auto A = frc::MakeMatrix<2, 2>(0.0, 1.0, 0.0, -kV / kA);
  auto B = frc::MakeMatrix<2, 1>(0.0, 1.0 / kA);
  auto C = frc::MakeMatrix<1, 2>(1.0, 0.0);
  auto D = frc::MakeMatrix<1, 1>(0.0);
  auto uMin = frc::MakeMatrix<1, 1>(-12.0);
  auto uMax = frc::MakeMatrix<1, 1>(12.0);

  return LinearSystem<2, 1, 1>(A, B, C, D, uMin, uMax);
}

LinearSystem<2, 2, 2> IdentifyDrivetrainSystem(double kVlinear, double kAlinear,
                                               double kVangular,
                                               double kAangular) {
  double c = 0.5 / (kAlinear * kAangular);
  double A1 = c * (-kAlinear * kVangular - kVlinear * kAangular);
  double A2 = c * (kAlinear * kVangular - kVlinear * kAangular);
  double B1 = c * (kAlinear + kAangular);
  double B2 = c * (kAangular - kAlinear);

  auto A = frc::MakeMatrix<2, 2>(A1, A2, A2, A1);
  auto B = frc::MakeMatrix<2, 2>(B1, B2, B2, B1);
  auto C = frc::MakeMatrix<2, 2>(1.0, 0.0, 0.0, 1.0);
  auto D = frc::MakeMatrix<2, 2>(0.0, 0.0, 0.0, 0.0);
  auto uMin = frc::MakeMatrix<2, 1>(-12.0, -12.0);
  auto uMax = frc::MakeMatrix<2, 1>(12.0, 12.0);

  return LinearSystem<2, 2, 2>(A, B, C, D, uMin, uMax);
}

}  // namespace frc
