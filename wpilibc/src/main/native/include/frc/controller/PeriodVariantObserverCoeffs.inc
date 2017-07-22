/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/controller/PeriodVariantObserverCoeffs.h"

namespace frc {

template <int States, int Inputs, int Outputs>
PeriodVariantObserverCoeffs<States, Inputs, Outputs>::
    PeriodVariantObserverCoeffs(
        const Eigen::Matrix<double, States, States>& Qcontinuous,
        const Eigen::Matrix<double, Outputs, Outputs>& Rcontinuous,
        const Eigen::Matrix<double, States, States>& PsteadyState)
    : Qcontinuous(Qcontinuous),
      Rcontinuous(Rcontinuous),
      PsteadyState(PsteadyState) {}

}  // namespace frc
