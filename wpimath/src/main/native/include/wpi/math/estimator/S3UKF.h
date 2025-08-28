// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "wpi/math/estimator/S3SigmaPoints.h"
#include "wpi/math/estimator/UnscentedKalmanFilter.h"

namespace wpi::math {

template <int States, int Inputs, int Outputs>
using S3UKF =
    UnscentedKalmanFilter<States, Inputs, Outputs, S3SigmaPoints<States>>;

// Because S3UKF is a type alias and not a class, we have to use
// UnscentedKalmanFilter instead
extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    UnscentedKalmanFilter<3, 3, 1, S3SigmaPoints<3>>;
extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    UnscentedKalmanFilter<5, 3, 3, S3SigmaPoints<5>>;

}  // namespace wpi::math
