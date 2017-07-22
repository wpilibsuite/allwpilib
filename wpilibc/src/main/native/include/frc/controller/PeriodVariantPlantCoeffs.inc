/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

template <int States, int Inputs, int Outputs>
PeriodVariantPlantCoeffs<States, Inputs, Outputs>::PeriodVariantPlantCoeffs(
    const Eigen::Matrix<double, States, States>& Acontinuous,
    const Eigen::Matrix<double, States, Inputs>& Bcontinuous,
    const Eigen::Matrix<double, Outputs, States>& C,
    const Eigen::Matrix<double, Outputs, Inputs>& D)
    : Acontinuous(Acontinuous), Bcontinuous(Bcontinuous), C(C), D(D) {}

}  // namespace frc
