// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DARE.h"

template Eigen::Matrix<double, 4, 4> frc::DARE<4, 1>(
    const Eigen::Matrix<double, 4, 4>& A, const Eigen::Matrix<double, 4, 1>& B,
    const Eigen::Matrix<double, 4, 4>& Q, const Eigen::Matrix<double, 1, 1>& R);
template Eigen::Matrix<double, 4, 4> frc::DARE<4, 1>(
    const Eigen::Matrix<double, 4, 4>& A, const Eigen::Matrix<double, 4, 1>& B,
    const Eigen::Matrix<double, 4, 4>& Q, const Eigen::Matrix<double, 1, 1>& R,
    const Eigen::Matrix<double, 4, 1>& N);
