// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/random/Normal.hpp"

#include <random>
#include <span>

#include <Eigen/Core>

namespace wpi::math {

Eigen::VectorXd Normal(const std::span<const double> stdDevs) {
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

}  // namespace wpi::math
