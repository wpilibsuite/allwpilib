/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/DifferentialDriveCoeffs.h"

#include <Eigen/Core>

frc::PeriodVariantPlantCoeffs<4, 2, 2> MakeDifferentialDrivePlantCoeffs() {
  Eigen::Matrix<double, 4, 4> Acontinuous;
  Acontinuous(0, 0) = 0.0;
  Acontinuous(0, 1) = 1.0;
  Acontinuous(0, 2) = 0.0;
  Acontinuous(0, 3) = 0.0;
  Acontinuous(1, 0) = 0.0;
  Acontinuous(1, 1) = -5.027892724619944;
  Acontinuous(1, 2) = 0.0;
  Acontinuous(1, 3) = -0.6998646867368526;
  Acontinuous(2, 0) = 0.0;
  Acontinuous(2, 1) = 0.0;
  Acontinuous(2, 2) = 0.0;
  Acontinuous(2, 3) = 1.0;
  Acontinuous(3, 0) = 0.0;
  Acontinuous(3, 1) = -0.6998646867368526;
  Acontinuous(3, 2) = 0.0;
  Acontinuous(3, 3) = -5.027892724619944;
  Eigen::Matrix<double, 4, 2> Bcontinuous;
  Bcontinuous(0, 0) = 0.0;
  Bcontinuous(0, 1) = 0.0;
  Bcontinuous(1, 0) = 1.7995488399804316;
  Bcontinuous(1, 1) = 0.25049076305735457;
  Bcontinuous(2, 0) = 0.0;
  Bcontinuous(2, 1) = 0.0;
  Bcontinuous(3, 0) = 0.25049076305735457;
  Bcontinuous(3, 1) = 1.7995488399804316;
  Eigen::Matrix<double, 2, 4> C;
  C(0, 0) = 1.0;
  C(0, 1) = 0.0;
  C(0, 2) = 0.0;
  C(0, 3) = 0.0;
  C(1, 0) = 0.0;
  C(1, 1) = 0.0;
  C(1, 2) = 1.0;
  C(1, 3) = 0.0;
  Eigen::Matrix<double, 2, 2> D;
  D(0, 0) = 0.0;
  D(0, 1) = 0.0;
  D(1, 0) = 0.0;
  D(1, 1) = 0.0;
  return frc::PeriodVariantPlantCoeffs<4, 2, 2>(Acontinuous, Bcontinuous, C, D);
}

frc::StateSpaceControllerCoeffs<4, 2, 2>
MakeDifferentialDriveControllerCoeffs() {
  Eigen::Matrix<double, 2, 4> K;
  K(0, 0) = 80.65341818675051;
  K(0, 1) = 12.71586889712588;
  K(0, 2) = -0.5333100328369462;
  K(0, 3) = -0.4931520992908445;
  K(1, 0) = -0.5333100328373179;
  K(1, 1) = -0.4931520992908412;
  K(1, 2) = 80.65341818675063;
  K(1, 3) = 12.715868897125851;
  Eigen::Matrix<double, 2, 4> Kff;
  Kff(0, 0) = 129.05358519143596;
  Kff(0, 1) = 1.272274192517092;
  Kff(0, 2) = 17.31489107452048;
  Kff(0, 3) = 0.16996589760178937;
  Kff(1, 0) = 17.314891074520485;
  Kff(1, 1) = 0.16996589760178946;
  Kff(1, 2) = 129.053585191436;
  Kff(1, 3) = 1.2722741925170924;
  Eigen::Matrix<double, 2, 1> Umin;
  Umin(0, 0) = -12.0;
  Umin(1, 0) = -12.0;
  Eigen::Matrix<double, 2, 1> Umax;
  Umax(0, 0) = 12.0;
  Umax(1, 0) = 12.0;
  return frc::StateSpaceControllerCoeffs<4, 2, 2>(K, Kff, Umin, Umax);
}

frc::PeriodVariantObserverCoeffs<4, 2, 2>
MakeDifferentialDriveObserverCoeffs() {
  Eigen::Matrix<double, 4, 4> Qcontinuous;
  Qcontinuous(0, 0) = 0.0025000000000000005;
  Qcontinuous(0, 1) = 0.0;
  Qcontinuous(0, 2) = 0.0;
  Qcontinuous(0, 3) = 0.0;
  Qcontinuous(1, 0) = 0.0;
  Qcontinuous(1, 1) = 1.0;
  Qcontinuous(1, 2) = 0.0;
  Qcontinuous(1, 3) = 0.0;
  Qcontinuous(2, 0) = 0.0;
  Qcontinuous(2, 1) = 0.0;
  Qcontinuous(2, 2) = 0.0025000000000000005;
  Qcontinuous(2, 3) = 0.0;
  Qcontinuous(3, 0) = 0.0;
  Qcontinuous(3, 1) = 0.0;
  Qcontinuous(3, 2) = 0.0;
  Qcontinuous(3, 3) = 1.0;
  Eigen::Matrix<double, 2, 2> Rcontinuous;
  Rcontinuous(0, 0) = 1e-08;
  Rcontinuous(0, 1) = 0.0;
  Rcontinuous(1, 0) = 0.0;
  Rcontinuous(1, 1) = 1e-08;
  Eigen::Matrix<double, 4, 4> PsteadyState;
  PsteadyState(0, 0) = 9.999963029693557e-09;
  PsteadyState(0, 1) = 1.4808967508114098e-07;
  PsteadyState(0, 2) = -9.883671001894896e-17;
  PsteadyState(0, 3) = -5.0939159960507185e-09;
  PsteadyState(1, 0) = 1.4808967507921014e-07;
  PsteadyState(1, 1) = 8.238974828958986;
  PsteadyState(1, 2) = -5.093915996150815e-09;
  PsteadyState(1, 3) = -0.26183323515195367;
  PsteadyState(2, 0) = -9.883769833075987e-17;
  PsteadyState(2, 1) = -5.093915996217903e-09;
  PsteadyState(2, 2) = 9.999963029993743e-09;
  PsteadyState(2, 3) = 1.480896750855879e-07;
  PsteadyState(3, 0) = -5.093915996044057e-09;
  PsteadyState(3, 1) = -0.26183323515195367;
  PsteadyState(3, 2) = 1.4808967508522786e-07;
  PsteadyState(3, 3) = 8.238974828958877;
  return frc::PeriodVariantObserverCoeffs<4, 2, 2>(Qcontinuous, Rcontinuous,
                                                   PsteadyState);
}

frc::PeriodVariantLoop<4, 2, 2> MakeDifferentialDriveLoop() {
  return frc::PeriodVariantLoop<4, 2, 2>(
      MakeDifferentialDrivePlantCoeffs(),
      MakeDifferentialDriveControllerCoeffs(),
      MakeDifferentialDriveObserverCoeffs());
}
