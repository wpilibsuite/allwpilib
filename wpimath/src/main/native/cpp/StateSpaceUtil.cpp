// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/StateSpaceUtil.h"

#include <limits>

namespace wpi::math {

template bool IsStabilizable<1, 1>(const Matrixd<1, 1>& A,
                                   const Matrixd<1, 1>& B);
template bool IsStabilizable<2, 1>(const Matrixd<2, 2>& A,
                                   const Matrixd<2, 1>& B);
template bool IsStabilizable<Eigen::Dynamic, Eigen::Dynamic>(
    const Eigen::MatrixXd& A, const Eigen::MatrixXd& B);

template bool IsDetectable<Eigen::Dynamic, Eigen::Dynamic>(
    const Eigen::MatrixXd& A, const Eigen::MatrixXd& C);

template Eigen::VectorXd ClampInputMaxMagnitude<Eigen::Dynamic>(
    const Eigen::VectorXd& u, const Eigen::VectorXd& umin,
    const Eigen::VectorXd& umax);

template Eigen::VectorXd DesaturateInputVector<Eigen::Dynamic>(
    const Eigen::VectorXd& u, double maxMagnitude);

Eigen::MatrixXd MakeCostMatrix(const std::span<const double> costs) {
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

Eigen::VectorXd MakeWhiteNoiseVector(const std::span<const double> stdDevs) {
  std::random_device rd;
  std::mt19937 gen{rd()};

  Eigen::VectorXd result{stdDevs.size()};
  for (size_t i = 0; i < stdDevs.size(); ++i) {
    // Passing a standard deviation of 0.0 to std::normal_distribution is
    // undefined behavior
    if (stdDevs[i] == 0.0) {
      result(i) = 0.0;
    } else {
      std::normal_distribution distr{0.0, stdDevs[i]};
      result(i) = distr(gen);
    }
  }
  return result;
}

Eigen::MatrixXd MakeCovMatrix(const std::span<const double> stdDevs) {
  Eigen::MatrixXd result{stdDevs.size(), stdDevs.size()};
  result.setZero();

  for (size_t i = 0; i < stdDevs.size(); ++i) {
    result(i, i) = std::pow(stdDevs[i], 2);
  }

  return result;
}

}  // namespace wpi::math
