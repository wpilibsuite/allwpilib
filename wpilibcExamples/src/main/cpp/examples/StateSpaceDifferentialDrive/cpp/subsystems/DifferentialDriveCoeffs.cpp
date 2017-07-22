/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/DifferentialDriveCoeffs.h"

#include <Eigen/Core>

frc::StateSpacePlantCoeffs<4, 2, 2> MakeDifferentialDrivePlantCoeffs() {
  Eigen::Matrix<double, 4, 4> A;
  A(0, 0) = 1.0;
  A(0, 1) = 0.004986437590193592;
  A(0, 2) = 0.0;
  A(0, 3) = -8.774526130757167e-06;
  A(1, 0) = 0.0;
  A(1, 1) = 0.974934867699476;
  A(1, 2) = 0.0;
  A(1, 3) = -0.003445714205898883;
  A(2, 0) = 0.0;
  A(2, 1) = -8.774526130757165e-06;
  A(2, 2) = 1.0;
  A(2, 3) = 0.004986437590193592;
  A(3, 0) = 0.0;
  A(3, 1) = -0.003445714205898883;
  A(3, 2) = 0.0;
  A(3, 3) = 0.974934867699476;
  Eigen::Matrix<double, 4, 2> B;
  B(0, 0) = 2.2749821266747515e-05;
  B(0, 1) = 3.1405181424541263e-06;
  B(1, 0) = 0.008971140043321736;
  B(1, 1) = 0.001233266368585487;
  B(2, 0) = 3.140518142454126e-06;
  B(2, 1) = 2.274982126674752e-05;
  B(3, 0) = 0.0012332663685854868;
  B(3, 1) = 0.008971140043321736;
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
  return frc::StateSpacePlantCoeffs<4, 2, 2>(A, B, C, D);
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

frc::StateSpaceObserverCoeffs<4, 2, 2> MakeDifferentialDriveObserverCoeffs() {
  Eigen::Matrix<double, 4, 2> K;
  K(0, 0) = 0.999996302984025;
  K(0, 1) = -9.883770254956401e-09;
  K(1, 0) = 14.808967508331385;
  K(1, 1) = -0.5093915996139777;
  K(2, 0) = -9.883770254887717e-09;
  K(2, 1) = 0.9999963029840249;
  K(3, 0) = -0.5093915996140141;
  K(3, 1) = 14.808967508331481;
  return frc::StateSpaceObserverCoeffs<4, 2, 2>(K);
}

frc::StateSpaceLoop<4, 2, 2> MakeDifferentialDriveLoop() {
  return frc::StateSpaceLoop<4, 2, 2>(MakeDifferentialDrivePlantCoeffs(),
                                      MakeDifferentialDriveControllerCoeffs(),
                                      MakeDifferentialDriveObserverCoeffs());
}
