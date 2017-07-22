/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/FlywheelCoeffs.h"

#include <Eigen/Core>

frc::PeriodVariantPlantCoeffs<1, 1, 1> MakeFlywheelPlantCoeffs() {
  Eigen::Matrix<double, 1, 1> Acontinuous;
  Acontinuous(0, 0) = -0.5001321900589898;
  Eigen::Matrix<double, 1, 1> Bcontinuous;
  Bcontinuous(0, 0) = 123.26388888888889;
  Eigen::Matrix<double, 1, 1> C;
  C(0, 0) = 1.0;
  Eigen::Matrix<double, 1, 1> D;
  D(0, 0) = 0.0;
  return frc::PeriodVariantPlantCoeffs<1, 1, 1>(Acontinuous, Bcontinuous, C, D);
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

frc::PeriodVariantObserverCoeffs<1, 1, 1> MakeFlywheelObserverCoeffs() {
  Eigen::Matrix<double, 1, 1> Qcontinuous;
  Qcontinuous(0, 0) = 1.0;
  Eigen::Matrix<double, 1, 1> Rcontinuous;
  Rcontinuous(0, 0) = 0.0001;
  Eigen::Matrix<double, 1, 1> PsteadyState;
  PsteadyState(0, 0) = 9.999000199441811e-05;
  return frc::PeriodVariantObserverCoeffs<1, 1, 1>(Qcontinuous, Rcontinuous,
                                                   PsteadyState);
}

frc::PeriodVariantLoop<1, 1, 1> MakeFlywheelLoop() {
  return frc::PeriodVariantLoop<1, 1, 1>(MakeFlywheelPlantCoeffs(),
                                         MakeFlywheelControllerCoeffs(),
                                         MakeFlywheelObserverCoeffs());
}
