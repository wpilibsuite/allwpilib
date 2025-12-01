// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/util/StateSpaceUtil.hpp"

#include <limits>
#include <span>

namespace wpi::math {

Eigen::MatrixXd CostMatrix(const std::span<const double> costs) {
  Eigen::MatrixXd result{costs.size(), costs.size()};
  result.setZero();

  for (size_t i = 0; i < costs.size(); ++i) {
    if (costs[i] == std::numeric_limits<double>::infinity()) {
      result(i, i) = 0.0;
    } else {
      result(i, i) = 1.0 / (std::pow(costs[i], 2));
    }
  }
  return result;
}

Eigen::MatrixXd CovarianceMatrix(const std::span<const double> stdDevs) {
  Eigen::MatrixXd result{stdDevs.size(), stdDevs.size()};
  result.setZero();

  for (size_t i = 0; i < stdDevs.size(); ++i) {
    result(i, i) = std::pow(stdDevs[i], 2);
  }

  return result;
}

template Eigen::VectorXd DesaturateInputVector<Eigen::Dynamic>(
    const Eigen::VectorXd& u, double maxMagnitude);

}  // namespace wpi::math
