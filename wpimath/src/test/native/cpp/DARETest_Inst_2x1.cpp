// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DARE.h"

template Eigen::Matrix<double, 2, 2> frc::DARE<2, 1>(
    const Eigen::Matrix<double, 2, 2>& A, const Eigen::Matrix<double, 2, 1>& B,
    const Eigen::Matrix<double, 2, 2>& Q, const Eigen::Matrix<double, 1, 1>& R);
template Eigen::Matrix<double, 2, 2> frc::DARE<2, 1>(
    const Eigen::Matrix<double, 2, 2>& A, const Eigen::Matrix<double, 2, 1>& B,
    const Eigen::Matrix<double, 2, 2>& Q, const Eigen::Matrix<double, 1, 1>& R,
    const Eigen::Matrix<double, 2, 1>& N);
