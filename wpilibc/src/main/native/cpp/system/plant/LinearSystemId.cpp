/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/system/plant/LinearSystemId.h"

namespace frc {

LinearSystem<1, 1, 1> IdentifyVelocitySystem(double kV, double kA) {
  Eigen::Matrix<double, 1, 1> A;
  A << -kV / kA;
  Eigen::Matrix<double, 1, 1> B;
  B << 1.0 / kA;
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

LinearSystem<2, 1, 1> IdentifyPositionSystem(double kV, double kA) {
  Eigen::Matrix<double, 2, 2> A;
  A << 0, 1, 0, -kV / kA;
  Eigen::Matrix<double, 2, 1> B;
  B << 0, 1.0 / kA;
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

LinearSystem<2, 2, 2> IdentifyDrivetrainSystem(double kVlinear, double kAlinear,
                                               double kVangular,
                                               double kAangular) {
  double c = 0.5 / (kAlinear * kAangular);
  double A1 = c * (-kAlinear * kVangular - kVlinear * kAangular);
  double A2 = c * (kAlinear * kVangular - kVlinear * kAangular);
  double B1 = c * (kAlinear + kAangular);
  double B2 = c * (kAangular - kAlinear);

  Eigen::Matrix<double, 2, 2> A;
  A << A1, A2, A2, A1;
  Eigen::Matrix<double, 2, 2> B;
  B << B1, B2, B2, B1;
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
