/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/SingleJointedArmCoeffs.h"

#include <Eigen/Core>

frc::PeriodVariantPlantCoeffs<2, 1, 1> MakeSingleJointedArmPlantCoeffs() {
  Eigen::Matrix<double, 2, 2> Acontinuous;
  Acontinuous(0, 0) = 0.0;
  Acontinuous(0, 1) = 1.0;
  Acontinuous(1, 0) = 0.0;
  Acontinuous(1, 1) = -0.000948744041441224;
  Eigen::Matrix<double, 2, 1> Bcontinuous;
  Bcontinuous(0, 0) = 0.0;
  Bcontinuous(1, 0) = 0.08974870179312819;
  Eigen::Matrix<double, 1, 2> C;
  C(0, 0) = 1.0;
  C(0, 1) = 0.0;
  Eigen::Matrix<double, 1, 1> D;
  D(0, 0) = 0.0;
  return frc::PeriodVariantPlantCoeffs<2, 1, 1>(Acontinuous, Bcontinuous, C, D);
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

frc::PeriodVariantObserverCoeffs<2, 1, 1> MakeSingleJointedArmObserverCoeffs() {
  Eigen::Matrix<double, 2, 2> Qcontinuous;
  Qcontinuous(0, 0) = 0.0003045025;
  Qcontinuous(0, 1) = 0.0;
  Qcontinuous(1, 0) = 0.0;
  Qcontinuous(1, 1) = 0.030461733182409998;
  Eigen::Matrix<double, 1, 1> Rcontinuous;
  Rcontinuous(0, 0) = 0.0001;
  Eigen::Matrix<double, 2, 2> PsteadyState;
  PsteadyState(0, 0) = 8.033915976431203e-05;
  PsteadyState(0, 1) = 0.0007738121910642457;
  PsteadyState(1, 0) = 0.000773812191064246;
  PsteadyState(1, 1) = 0.62614228094563;
  return frc::PeriodVariantObserverCoeffs<2, 1, 1>(Qcontinuous, Rcontinuous,
                                                   PsteadyState);
}

frc::PeriodVariantLoop<2, 1, 1> MakeSingleJointedArmLoop() {
  return frc::PeriodVariantLoop<2, 1, 1>(MakeSingleJointedArmPlantCoeffs(),
                                         MakeSingleJointedArmControllerCoeffs(),
                                         MakeSingleJointedArmObserverCoeffs());
}
