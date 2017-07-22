/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

template <int States, int Inputs, int Outputs>
StateSpaceControllerCoeffs<States, Inputs, Outputs>::StateSpaceControllerCoeffs(
    const Eigen::Matrix<double, Inputs, States>& K,
    const Eigen::Matrix<double, Inputs, States>& Kff,
    const Eigen::Matrix<double, Inputs, 1>& Umin,
    const Eigen::Matrix<double, Inputs, 1>& Umax)
    : K(K), Kff(Kff), Umin(Umin), Umax(Umax) {}

}  // namespace frc
