// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/system/LinearSystemUtil.hpp"

#include <Eigen/Core>

namespace wpi::math {

template bool IsStabilizable<1, 1>(const Eigen::Matrix<double, 1, 1>& A,
                                   const Eigen::Matrix<double, 1, 1>& B);
template bool IsStabilizable<2, 1>(const Eigen::Matrix<double, 2, 2>& A,
                                   const Eigen::Matrix<double, 2, 1>& B);
template bool IsStabilizable<Eigen::Dynamic, Eigen::Dynamic>(
    const Eigen::MatrixXd& A, const Eigen::MatrixXd& B);

template bool IsDetectable<Eigen::Dynamic, Eigen::Dynamic>(
    const Eigen::MatrixXd& A, const Eigen::MatrixXd& C);

}  // namespace wpi::math
