// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "wpimath/estimator/MerweScaledSigmaPoints.h"
#include "wpimath/estimator/UnscentedKalmanFilter.h"

namespace wpimath {

template <int States, int Inputs, int Outputs>
using MerweUKF = UnscentedKalmanFilter<States, Inputs, Outputs,
                                       MerweScaledSigmaPoints<States>>;

// Because MerweUKF is a type alias and not a class, we have to use
// UnscentedKalmanFilter instead
extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    UnscentedKalmanFilter<3, 3, 1, MerweScaledSigmaPoints<3>>;
extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    UnscentedKalmanFilter<5, 3, 3, MerweScaledSigmaPoints<5>>;

}  // namespace wpimath
