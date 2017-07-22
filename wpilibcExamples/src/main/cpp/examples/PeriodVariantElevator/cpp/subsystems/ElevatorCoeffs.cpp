/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/ElevatorCoeffs.h"

#include <Eigen/Core>

frc::PeriodVariantPlantCoeffs<2, 1, 1> MakeElevatorPlantCoeffs() {
  Eigen::Matrix<double, 2, 2> Acontinuous;
  Acontinuous(0, 0) = 0.0;
  Acontinuous(0, 1) = 1.0;
  Acontinuous(1, 0) = 0.0;
  Acontinuous(1, 1) = -164.20904979369794;
  Eigen::Matrix<double, 2, 1> Bcontinuous;
  Bcontinuous(0, 0) = 0.0;
  Bcontinuous(1, 0) = 21.457377744727182;
  Eigen::Matrix<double, 1, 2> C;
  C(0, 0) = 1.0;
  C(0, 1) = 0.0;
  Eigen::Matrix<double, 1, 1> D;
  D(0, 0) = 0.0;
  return frc::PeriodVariantPlantCoeffs<2, 1, 1>(Acontinuous, Bcontinuous, C, D);
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

frc::PeriodVariantObserverCoeffs<2, 1, 1> MakeElevatorObserverCoeffs() {
  Eigen::Matrix<double, 2, 2> Qcontinuous;
  Qcontinuous(0, 0) = 0.0025000000000000005;
  Qcontinuous(0, 1) = 0.0;
  Qcontinuous(1, 0) = 0.0;
  Qcontinuous(1, 1) = 1.0;
  Eigen::Matrix<double, 1, 1> Rcontinuous;
  Rcontinuous(0, 0) = 1e-08;
  Eigen::Matrix<double, 2, 2> PsteadyState;
  PsteadyState(0, 0) = 9.99996023173184e-09;
  PsteadyState(0, 1) = 7.34757941922686e-09;
  PsteadyState(1, 0) = 7.347579419241315e-09;
  PsteadyState(1, 1) = 1.2335349474145199;
  return frc::PeriodVariantObserverCoeffs<2, 1, 1>(Qcontinuous, Rcontinuous,
                                                   PsteadyState);
}

frc::PeriodVariantLoop<2, 1, 1> MakeElevatorLoop() {
  return frc::PeriodVariantLoop<2, 1, 1>(MakeElevatorPlantCoeffs(),
                                         MakeElevatorControllerCoeffs(),
                                         MakeElevatorObserverCoeffs());
}
