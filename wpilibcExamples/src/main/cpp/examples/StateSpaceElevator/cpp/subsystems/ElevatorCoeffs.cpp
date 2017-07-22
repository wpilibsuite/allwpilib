/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/ElevatorCoeffs.h"

#include <Eigen/Core>

frc::StateSpacePlantCoeffs<2, 1, 1> MakeElevatorPlantCoeffs() {
  Eigen::Matrix<double, 2, 2> A;
  A(0, 0) = 1.0;
  A(0, 1) = 0.0034323689390278016;
  A(1, 0) = 0.0;
  A(1, 1) = 0.4363739579808415;
  Eigen::Matrix<double, 2, 1> B;
  B(0, 0) = 0.00021137763582757403;
  B(1, 0) = 0.07364963688398798;
  Eigen::Matrix<double, 1, 2> C;
  C(0, 0) = 1.0;
  C(0, 1) = 0.0;
  Eigen::Matrix<double, 1, 1> D;
  D(0, 0) = 0.0;
  return frc::StateSpacePlantCoeffs<2, 1, 1>(A, B, C, D);
}

frc::StateSpaceControllerCoeffs<2, 1, 1> MakeElevatorControllerCoeffs() {
  Eigen::Matrix<double, 1, 2> K;
  K(0, 0) = 232.76812610676956;
  K(0, 1) = 5.540693882702349;
  Eigen::Matrix<double, 1, 2> Kff;
  Kff(0, 0) = 12.902151500051774;
  Kff(0, 1) = 11.23863895630079;
  Eigen::Matrix<double, 1, 1> Umin;
  Umin(0, 0) = -12.0;
  Eigen::Matrix<double, 1, 1> Umax;
  Umax(0, 0) = 12.0;
  return frc::StateSpaceControllerCoeffs<2, 1, 1>(K, Kff, Umin, Umax);
}

frc::StateSpaceObserverCoeffs<2, 1, 1> MakeElevatorObserverCoeffs() {
  Eigen::Matrix<double, 2, 1> K;
  K(0, 0) = 0.9999960231492777;
  K(1, 0) = 0.7347579419051207;
  return frc::StateSpaceObserverCoeffs<2, 1, 1>(K);
}

frc::StateSpaceLoop<2, 1, 1> MakeElevatorLoop() {
  return frc::StateSpaceLoop<2, 1, 1>(MakeElevatorPlantCoeffs(),
                                      MakeElevatorControllerCoeffs(),
                                      MakeElevatorObserverCoeffs());
}
