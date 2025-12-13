// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/linalg/DARE.hpp"
#include "wpi/util/expected"

template wpi::util::expected<Eigen::Matrix<double, 4, 4>, wpi::math::DAREError>
wpi::math::DARE<4, 1>(const Eigen::Matrix<double, 4, 4>& A,
                      const Eigen::Matrix<double, 4, 1>& B,
                      const Eigen::Matrix<double, 4, 4>& Q,
                      const Eigen::Matrix<double, 1, 1>& R,
                      bool checkPreconditions);
template wpi::util::expected<Eigen::Matrix<double, 4, 4>, wpi::math::DAREError>
wpi::math::DARE<4, 1>(const Eigen::Matrix<double, 4, 4>& A,
                      const Eigen::Matrix<double, 4, 1>& B,
                      const Eigen::Matrix<double, 4, 4>& Q,
                      const Eigen::Matrix<double, 1, 1>& R,
                      const Eigen::Matrix<double, 4, 1>& N,
                      bool checkPreconditions);
