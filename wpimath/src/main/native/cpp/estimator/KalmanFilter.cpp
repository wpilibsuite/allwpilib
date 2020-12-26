// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/estimator/KalmanFilter.h"

namespace frc {

KalmanFilter<1, 1, 1>::KalmanFilter(
    LinearSystem<1, 1, 1>& plant, const std::array<double, 1>& stateStdDevs,
    const std::array<double, 1>& measurementStdDevs, units::second_t dt)
    : detail::KalmanFilterImpl<1, 1, 1>{plant, stateStdDevs, measurementStdDevs,
                                        dt} {}

KalmanFilter<2, 1, 1>::KalmanFilter(
    LinearSystem<2, 1, 1>& plant, const std::array<double, 2>& stateStdDevs,
    const std::array<double, 1>& measurementStdDevs, units::second_t dt)
    : detail::KalmanFilterImpl<2, 1, 1>{plant, stateStdDevs, measurementStdDevs,
                                        dt} {}

}  // namespace frc
