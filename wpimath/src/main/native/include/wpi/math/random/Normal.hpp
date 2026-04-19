// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <concepts>
#include <random>
#include <span>

#include <Eigen/Core>

#include "wpi/util/Algorithm.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * Creates a vector of normally distributed random values with the given
 * standard deviations for each element.
 *
 * @param stdDevs An array whose elements are the standard deviations of each
 *     element of the random vector.
 * @return Vector of normally distributed values.
 */
template <std::same_as<double>... Ts>
Eigen::Vector<double, sizeof...(Ts)> Normal(Ts... stdDevs) {
  std::random_device rd;
  std::mt19937 gen{rd()};

  Eigen::Vector<double, sizeof...(Ts)> result;
  wpi::util::for_each(
      [&](int i, double stdDev) {
        // Passing a standard deviation of 0.0 to std::normal_distribution is
        // undefined behavior
        if (stdDev == 0.0) {
          result(i) = 0.0;
        } else {
          std::normal_distribution distr{0.0, stdDev};
          result(i) = distr(gen);
        }
      },
      stdDevs...);
  return result;
}

/**
 * Creates a vector of normally distributed random values with the given
 * standard deviations for each element.
 *
 * @param stdDevs An array whose elements are the standard deviations of each
 *     element of the random vector.
 * @return Vector of normally distributed values.
 */
template <int N>
Eigen::Vector<double, N> Normal(const std::array<double, N>& stdDevs) {
  std::random_device rd;
  std::mt19937 gen{rd()};

  Eigen::Vector<double, N> result;
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

/**
 * Creates a vector of normally distributed random values with the given
 * standard deviations for each element.
 *
 * @param stdDevs A possibly variable length container whose elements are the
 *     standard deviations of each element of the random vector.
 * @return Vector of normally distributed values.
 */
WPILIB_DLLEXPORT Eigen::VectorXd Normal(const std::span<const double> stdDevs);

}  // namespace wpi::math
