/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

template <int States, int Inputs, int Outputs>
StateSpaceObserverCoeffs<States, Inputs, Outputs>::StateSpaceObserverCoeffs(
    const Eigen::Matrix<double, States, Outputs>& K)
    : K(K) {}

}  // namespace frc
