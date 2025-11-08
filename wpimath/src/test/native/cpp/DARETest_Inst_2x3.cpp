// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/linalg/DARE.hpp"

template wpi::util::expected<Eigen::Matrix<double, 2, 2>, wpi::math::DAREError>
wpi::math::DARE<2, 3>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 3>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 3, 3>& R,
                      bool checkPreconditions);
template wpi::util::expected<Eigen::Matrix<double, 2, 2>, wpi::math::DAREError>
wpi::math::DARE<2, 3>(const Eigen::Matrix<double, 2, 2>& A,
                      const Eigen::Matrix<double, 2, 3>& B,
                      const Eigen::Matrix<double, 2, 2>& Q,
                      const Eigen::Matrix<double, 3, 3>& R,
                      const Eigen::Matrix<double, 2, 3>& N,
                      bool checkPreconditions);
