// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/KalmanFilter.hpp"

namespace wpi::math {

template class EXPORT_TEMPLATE_DEFINE(WPILIB_DLLEXPORT) KalmanFilter<1, 1, 1>;
template class EXPORT_TEMPLATE_DEFINE(WPILIB_DLLEXPORT) KalmanFilter<2, 1, 1>;

}  // namespace wpi::math
