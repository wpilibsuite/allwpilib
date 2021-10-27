// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/LinearQuadraticRegulator.h"

namespace frc {

LinearQuadraticRegulator<2, 1>::LinearQuadraticRegulator(
    const Eigen::Matrix<double, 2, 2>& A, const Eigen::Matrix<double, 2, 1>& B,
    const wpi::array<double, 2>& Qelems, const wpi::array<double, 1>& Relems,
    units::second_t dt)
    : LinearQuadraticRegulator(A, B, MakeCostMatrix(Qelems),
                               MakeCostMatrix(Relems), dt) {}

}  // namespace frc
