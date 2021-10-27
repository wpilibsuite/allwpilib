// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/LinearQuadraticRegulator.h"

namespace frc {

LinearQuadraticRegulator<2, 1>::LinearQuadraticRegulator(
    const Eigen::Matrix<double, 2, 2>& A, const Eigen::Matrix<double, 2, 1>& B,
    const Eigen::Matrix<double, 2, 2>& Q, const Eigen::Matrix<double, 1, 1>& R,
    const Eigen::Matrix<double, 2, 1>& N, units::second_t dt)
    : detail::LinearQuadraticRegulatorImpl<2, 1>(A, B, Q, R, N, dt) {}

}  // namespace frc
