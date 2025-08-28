// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/MerweUKF.h"

namespace wpi::math {

template class EXPORT_TEMPLATE_DEFINE(WPILIB_DLLEXPORT)
    UnscentedKalmanFilter<3, 3, 1, MerweScaledSigmaPoints<3>>;
template class EXPORT_TEMPLATE_DEFINE(WPILIB_DLLEXPORT)
    UnscentedKalmanFilter<5, 3, 3, MerweScaledSigmaPoints<5>>;

}  // namespace wpi::math
