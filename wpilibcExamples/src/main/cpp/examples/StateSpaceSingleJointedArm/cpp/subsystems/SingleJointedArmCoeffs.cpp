/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/SingleJointedArmCoeffs.h"

#include <Eigen/Core>

frc::StateSpacePlantCoeffs<2, 1, 1> MakeSingleJointedArmPlantCoeffs() {
  Eigen::Matrix<double, 2, 2> A;
  A(0, 0) = 1.0;
  A(0, 1) = 0.005049987902346862;
  A(1, 0) = 0.0;
  A(1, 1) = 0.9999952088540683;
  Eigen::Matrix<double, 2, 1> B;
  B(0, 0) = 1.1444063060619785e-06;
  B(1, 0) = 0.0004532298583066335;
  Eigen::Matrix<double, 1, 2> C;
  C(0, 0) = 1.0;
  C(0, 1) = 0.0;
  Eigen::Matrix<double, 1, 1> D;
  D(0, 0) = 0.0;
  return frc::StateSpacePlantCoeffs<2, 1, 1>(A, B, C, D);
}

frc::StateSpaceControllerCoeffs<2, 1, 1>
MakeSingleJointedArmControllerCoeffs() {
  Eigen::Matrix<double, 1, 2> K;
  K(0, 0) = 659.4457876860411;
  K(0, 1) = 179.11558938712352;
  Eigen::Matrix<double, 1, 2> Kff;
  Kff(0, 0) = 0.5390980036447967;
  Kff(0, 1) = 8.538202281633405;
  Eigen::Matrix<double, 1, 1> Umin;
  Umin(0, 0) = -12.0;
  Eigen::Matrix<double, 1, 1> Umax;
  Umax(0, 0) = 12.0;
  return frc::StateSpaceControllerCoeffs<2, 1, 1>(K, Kff, Umin, Umax);
}

frc::StateSpaceObserverCoeffs<2, 1, 1> MakeSingleJointedArmObserverCoeffs() {
  Eigen::Matrix<double, 2, 1> K;
  K(0, 0) = 0.8033915976431203;
  K(1, 0) = 7.738121910642457;
  return frc::StateSpaceObserverCoeffs<2, 1, 1>(K);
}

frc::StateSpaceLoop<2, 1, 1> MakeSingleJointedArmLoop() {
  return frc::StateSpaceLoop<2, 1, 1>(MakeSingleJointedArmPlantCoeffs(),
                                      MakeSingleJointedArmControllerCoeffs(),
                                      MakeSingleJointedArmObserverCoeffs());
}
