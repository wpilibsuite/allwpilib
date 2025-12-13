// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <limits>
#include <span>

#include <Eigen/Core>

#include "wpi/util/Algorithm.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * Creates a cost matrix from the given vector for use with LQR.
 *
 * The cost matrix is constructed using Bryson's rule. The inverse square of
 * each tolerance is placed on the cost matrix diagonal. If a tolerance is
 * infinity, its cost matrix entry is set to zero.
 *
 * @param tolerances An array. For a Q matrix, its elements are the maximum
 *     allowed excursions of the states from the reference. For an R matrix, its
 *     elements are the maximum allowed excursions of the control inputs from no
 *     actuation.
 * @return State excursion or control effort cost matrix.
 */
template <std::same_as<double>... Ts>
constexpr Eigen::Matrix<double, sizeof...(Ts), sizeof...(Ts)> CostMatrix(
    Ts... tolerances) {
  Eigen::Matrix<double, sizeof...(Ts), sizeof...(Ts)> result;

  for (int row = 0; row < result.rows(); ++row) {
    for (int col = 0; col < result.cols(); ++col) {
      if (row != col) {
        result(row, col) = 0.0;
      }
    }
  }

  wpi::util::for_each(
      [&](int i, double tolerance) {
        if (tolerance == std::numeric_limits<double>::infinity()) {
          result(i, i) = 0.0;
        } else {
          result(i, i) = 1.0 / (tolerance * tolerance);
        }
      },
      tolerances...);

  return result;
}

/**
 * Creates a cost matrix from the given vector for use with LQR.
 *
 * The cost matrix is constructed using Bryson's rule. The inverse square of
 * each element in the input is placed on the cost matrix diagonal. If a
 * tolerance is infinity, its cost matrix entry is set to zero.
 *
 * @param costs An array. For a Q matrix, its elements are the maximum allowed
 *     excursions of the states from the reference. For an R matrix, its
 *     elements are the maximum allowed excursions of the control inputs from no
 *     actuation.
 * @return State excursion or control effort cost matrix.
 */
template <size_t N>
constexpr Eigen::Matrix<double, N, N> CostMatrix(
    const std::array<double, N>& costs) {
  Eigen::Matrix<double, N, N> result;

  for (int row = 0; row < result.rows(); ++row) {
    for (int col = 0; col < result.cols(); ++col) {
      if (row == col) {
        if (costs[row] == std::numeric_limits<double>::infinity()) {
          result(row, col) = 0.0;
        } else {
          result(row, col) = 1.0 / (costs[row] * costs[row]);
        }
      } else {
        result(row, col) = 0.0;
      }
    }
  }

  return result;
}

/**
 * Creates a cost matrix from the given vector for use with LQR.
 *
 * The cost matrix is constructed using Bryson's rule. The inverse square of
 * each element in the input is placed on the cost matrix diagonal. If a
 * tolerance is infinity, its cost matrix entry is set to zero.
 *
 * @param costs A possibly variable length container. For a Q matrix, its
 *     elements are the maximum allowed excursions of the states from the
 *     reference. For an R matrix, its elements are the maximum allowed
 *     excursions of the control inputs from no actuation.
 * @return State excursion or control effort cost matrix.
 */
WPILIB_DLLEXPORT Eigen::MatrixXd CostMatrix(
    const std::span<const double> costs);

/**
 * Creates a covariance matrix from the given vector for use with Kalman
 * filters.
 *
 * Each element is squared and placed on the covariance matrix diagonal.
 *
 * @param stdDevs An array. For a Q matrix, its elements are the standard
 *     deviations of each state from how the model behaves. For an R matrix, its
 *     elements are the standard deviations for each output measurement.
 * @return Process noise or measurement noise covariance matrix.
 */
template <std::same_as<double>... Ts>
constexpr Eigen::Matrix<double, sizeof...(Ts), sizeof...(Ts)> CovarianceMatrix(
    Ts... stdDevs) {
  Eigen::Matrix<double, sizeof...(Ts), sizeof...(Ts)> result;

  for (int row = 0; row < result.rows(); ++row) {
    for (int col = 0; col < result.cols(); ++col) {
      if (row != col) {
        result(row, col) = 0.0;
      }
    }
  }

  wpi::util::for_each(
      [&](int i, double stdDev) { result(i, i) = stdDev * stdDev; },
      stdDevs...);

  return result;
}

/**
 * Creates a covariance matrix from the given vector for use with Kalman
 * filters.
 *
 * Each element is squared and placed on the covariance matrix diagonal.
 *
 * @param stdDevs An array. For a Q matrix, its elements are the standard
 *     deviations of each state from how the model behaves. For an R matrix, its
 *     elements are the standard deviations for each output measurement.
 * @return Process noise or measurement noise covariance matrix.
 */
template <size_t N>
constexpr Eigen::Matrix<double, N, N> CovarianceMatrix(
    const std::array<double, N>& stdDevs) {
  Eigen::Matrix<double, N, N> result;

  for (int row = 0; row < result.rows(); ++row) {
    for (int col = 0; col < result.cols(); ++col) {
      if (row == col) {
        result(row, col) = stdDevs[row] * stdDevs[row];
      } else {
        result(row, col) = 0.0;
      }
    }
  }

  return result;
}

/**
 * Creates a covariance matrix from the given vector for use with Kalman
 * filters.
 *
 * Each element is squared and placed on the covariance matrix diagonal.
 *
 * @param stdDevs A possibly variable length container. For a Q matrix, its
 *     elements are the standard deviations of each state from how the model
 *     behaves. For an R matrix, its elements are the standard deviations for
 *     each output measurement.
 * @return Process noise or measurement noise covariance matrix.
 */
WPILIB_DLLEXPORT Eigen::MatrixXd CovarianceMatrix(
    const std::span<const double> stdDevs);

/**
 * Renormalize all inputs if any exceeds the maximum magnitude. Useful for
 * systems such as differential drivetrains.
 *
 * @tparam Inputs Number of inputs.
 * @param u The input vector.
 * @param maxMagnitude The maximum magnitude any input can have.
 * @return The normalizedInput
 */
template <int Inputs>
Eigen::Vector<double, Inputs> DesaturateInputVector(
    const Eigen::Vector<double, Inputs>& u, double maxMagnitude) {
  return u * std::min(1.0, maxMagnitude / u.template lpNorm<Eigen::Infinity>());
}

extern template WPILIB_DLLEXPORT Eigen::VectorXd
DesaturateInputVector<Eigen::Dynamic>(const Eigen::VectorXd& u,
                                      double maxMagnitude);

}  // namespace wpi::math
