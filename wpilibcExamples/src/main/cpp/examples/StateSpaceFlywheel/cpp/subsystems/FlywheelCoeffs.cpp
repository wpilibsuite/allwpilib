/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/FlywheelCoeffs.h"

#include <Eigen/Core>

frc::StateSpacePlantCoeffs<1, 1, 1> MakeFlywheelPlantCoeffs() {
  Eigen::Matrix<double, 1, 1> A;
  A(0, 0) = 0.9974775192550039;
  Eigen::Matrix<double, 1, 1> B;
  B(0, 0) = 0.6216972081698791;
  Eigen::Matrix<double, 1, 1> C;
  C(0, 0) = 1.0;
  Eigen::Matrix<double, 1, 1> D;
  D(0, 0) = 0.0;
  return frc::StateSpacePlantCoeffs<1, 1, 1>(A, B, C, D);
}

frc::StateSpaceControllerCoeffs<1, 1, 1> MakeFlywheelControllerCoeffs() {
  Eigen::Matrix<double, 1, 1> K;
  K(0, 0) = 0.8623214751211266;
  Eigen::Matrix<double, 1, 1> Kff;
  Kff(0, 0) = 0.6200031001383457;
  Eigen::Matrix<double, 1, 1> Umin;
  Umin(0, 0) = -12.0;
  Eigen::Matrix<double, 1, 1> Umax;
  Umax(0, 0) = 12.0;
  return frc::StateSpaceControllerCoeffs<1, 1, 1>(K, Kff, Umin, Umax);
}

frc::StateSpaceObserverCoeffs<1, 1, 1> MakeFlywheelObserverCoeffs() {
  Eigen::Matrix<double, 1, 1> K;
  K(0, 0) = 0.9999000199446406;
  return frc::StateSpaceObserverCoeffs<1, 1, 1>(K);
}

frc::StateSpaceLoop<1, 1, 1> MakeFlywheelLoop() {
  return frc::StateSpaceLoop<1, 1, 1>(MakeFlywheelPlantCoeffs(),
                                      MakeFlywheelControllerCoeffs(),
                                      MakeFlywheelObserverCoeffs());
}
