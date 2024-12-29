// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <cmath>
#include <concepts>
#include <limits>
#include <random>

#include <Eigen/Eigenvalues>
#include <Eigen/QR>
#include <wpi/Algorithm.h>
#include <wpi/SymbolExports.h>

#include "frc/EigenCore.h"
#include "frc/geometry/Pose2d.h"

namespace frc {

/**
 * Creates a cost matrix from the given vector for use with LQR.
 *
 * The cost matrix is constructed using Bryson's rule. The inverse square of
 * each tolerance is placed on the cost matrix diagonal. If a tolerance is
 * infinity, its cost matrix entry is set to zero.
 *
 * @param tolerances An array. For a Q matrix, its elements are the maximum
 *                   allowed excursions of the states from the reference. For an
 *                   R matrix, its elements are the maximum allowed excursions
 *                   of the control inputs from no actuation.
 * @return State excursion or control effort cost matrix.
 */
template <std::same_as<double>... Ts>
constexpr Matrixd<sizeof...(Ts), sizeof...(Ts)> MakeCostMatrix(
    Ts... tolerances) {
  Matrixd<sizeof...(Ts), sizeof...(Ts)> result;

  for (int row = 0; row < result.rows(); ++row) {
    for (int col = 0; col < result.cols(); ++col) {
      if (row != col) {
        result(row, col) = 0.0;
      }
    }
  }

  wpi::for_each(
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
 * Creates a covariance matrix from the given vector for use with Kalman
 * filters.
 *
 * Each element is squared and placed on the covariance matrix diagonal.
 *
 * @param stdDevs An array. For a Q matrix, its elements are the standard
 *                deviations of each state from how the model behaves. For an R
 *                matrix, its elements are the standard deviations for each
 *                output measurement.
 * @return Process noise or measurement noise covariance matrix.
 */
template <std::same_as<double>... Ts>
constexpr Matrixd<sizeof...(Ts), sizeof...(Ts)> MakeCovMatrix(Ts... stdDevs) {
  Matrixd<sizeof...(Ts), sizeof...(Ts)> result;

  for (int row = 0; row < result.rows(); ++row) {
    for (int col = 0; col < result.cols(); ++col) {
      if (row != col) {
        result(row, col) = 0.0;
      }
    }
  }

  wpi::for_each([&](int i, double stdDev) { result(i, i) = stdDev * stdDev; },
                stdDevs...);

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
 *              excursions of the states from the reference. For an R matrix,
 *              its elements are the maximum allowed excursions of the control
 *              inputs from no actuation.
 * @return State excursion or control effort cost matrix.
 */
template <size_t N>
constexpr Matrixd<N, N> MakeCostMatrix(const std::array<double, N>& costs) {
  Matrixd<N, N> result;

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
 * Creates a covariance matrix from the given vector for use with Kalman
 * filters.
 *
 * Each element is squared and placed on the covariance matrix diagonal.
 *
 * @param stdDevs An array. For a Q matrix, its elements are the standard
 *                deviations of each state from how the model behaves. For an R
 *                matrix, its elements are the standard deviations for each
 *                output measurement.
 * @return Process noise or measurement noise covariance matrix.
 */
template <size_t N>
constexpr Matrixd<N, N> MakeCovMatrix(const std::array<double, N>& stdDevs) {
  Matrixd<N, N> result;

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

template <std::same_as<double>... Ts>
Vectord<sizeof...(Ts)> MakeWhiteNoiseVector(Ts... stdDevs) {
  std::random_device rd;
  std::mt19937 gen{rd()};

  Vectord<sizeof...(Ts)> result;
  wpi::for_each(
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
 * Creates a vector of normally distributed white noise with the given noise
 * intensities for each element.
 *
 * @param stdDevs An array whose elements are the standard deviations of each
 *                element of the noise vector.
 * @return White noise vector.
 */
template <int N>
Vectord<N> MakeWhiteNoiseVector(const std::array<double, N>& stdDevs) {
  std::random_device rd;
  std::mt19937 gen{rd()};

  Vectord<N> result;
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
 * Converts a Pose2d into a vector of [x, y, theta].
 *
 * @param pose The pose that is being represented.
 *
 * @return The vector.
 * @deprecated Create the vector manually instead. If you were using this as an
 *     intermediate step for constructing affine transformations, use
 *     Pose2d.ToMatrix() instead.
 */
[[deprecated("Use Pose2d.ToMatrix() instead.")]]
WPILIB_DLLEXPORT constexpr Eigen::Vector3d PoseTo3dVector(const Pose2d& pose) {
  return Eigen::Vector3d{{pose.Translation().X().value(),
                          pose.Translation().Y().value(),
                          pose.Rotation().Radians().value()}};
}

/**
 * Converts a Pose2d into a vector of [x, y, std::cos(theta), std::sin(theta)].
 *
 * @param pose The pose that is being represented.
 *
 * @return The vector.
 * @deprecated Create the vector manually instead. If you were using this as an
 *     intermediate step for constructing affine transformations, use
 *     Pose2d.ToMatrix() instead.
 */
[[deprecated("Use Pose2d.ToMatrix() instead.")]]
WPILIB_DLLEXPORT constexpr Eigen::Vector4d PoseTo4dVector(const Pose2d& pose) {
  return Eigen::Vector4d{{pose.Translation().X().value(),
                          pose.Translation().Y().value(), pose.Rotation().Cos(),
                          pose.Rotation().Sin()}};
}

/**
 * Returns true if (A, B) is a stabilizable pair.
 *
 * (A, B) is stabilizable if and only if the uncontrollable eigenvalues of A, if
 * any, have absolute values less than one, where an eigenvalue is
 * uncontrollable if rank([λI - A, B]) < n where n is the number of states.
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 * @param A System matrix.
 * @param B Input matrix.
 */
template <int States, int Inputs>
bool IsStabilizable(const Matrixd<States, States>& A,
                    const Matrixd<States, Inputs>& B) {
  Eigen::EigenSolver<Matrixd<States, States>> es{A, false};

  for (int i = 0; i < A.rows(); ++i) {
    if (std::norm(es.eigenvalues()[i]) < 1) {
      continue;
    }

    if constexpr (States != Eigen::Dynamic && Inputs != Eigen::Dynamic) {
      Eigen::Matrix<std::complex<double>, States, States + Inputs> E;
      E << es.eigenvalues()[i] * Eigen::Matrix<std::complex<double>, States,
                                               States>::Identity() -
               A,
          B;

      Eigen::ColPivHouseholderQR<
          Eigen::Matrix<std::complex<double>, States, States + Inputs>>
          qr{E};
      if (qr.rank() < States) {
        return false;
      }
    } else {
      Eigen::MatrixXcd E{A.rows(), A.rows() + B.cols()};
      E << es.eigenvalues()[i] *
                   Eigen::MatrixXcd::Identity(A.rows(), A.rows()) -
               A,
          B;

      Eigen::ColPivHouseholderQR<Eigen::MatrixXcd> qr{E};
      if (qr.rank() < A.rows()) {
        return false;
      }
    }
  }
  return true;
}

extern template WPILIB_DLLEXPORT bool IsStabilizable<1, 1>(
    const Matrixd<1, 1>& A, const Matrixd<1, 1>& B);
extern template WPILIB_DLLEXPORT bool IsStabilizable<2, 1>(
    const Matrixd<2, 2>& A, const Matrixd<2, 1>& B);
extern template WPILIB_DLLEXPORT bool
IsStabilizable<Eigen::Dynamic, Eigen::Dynamic>(const Eigen::MatrixXd& A,
                                               const Eigen::MatrixXd& B);

/**
 * Returns true if (A, C) is a detectable pair.
 *
 * (A, C) is detectable if and only if the unobservable eigenvalues of A, if
 * any, have absolute values less than one, where an eigenvalue is unobservable
 * if rank([λI - A; C]) < n where n is the number of states.
 *
 * @tparam States Number of states.
 * @tparam Outputs Number of outputs.
 * @param A System matrix.
 * @param C Output matrix.
 */
template <int States, int Outputs>
bool IsDetectable(const Matrixd<States, States>& A,
                  const Matrixd<Outputs, States>& C) {
  return IsStabilizable<States, Outputs>(A.transpose(), C.transpose());
}

/**
 * Converts a Pose2d into a vector of [x, y, theta].
 *
 * @param pose The pose that is being represented.
 *
 * @return The vector.
 * @deprecated Create the vector manually instead. If you were using this as an
 *     intermediate step for constructing affine transformations, use
 *     Pose2d.ToMatrix() instead.
 */
[[deprecated("Use Pose2d.ToMatrix() instead.")]]
WPILIB_DLLEXPORT constexpr Eigen::Vector3d PoseToVector(const Pose2d& pose) {
  return Eigen::Vector3d{
      {pose.X().value(), pose.Y().value(), pose.Rotation().Radians().value()}};
}

/**
 * Clamps input vector between system's minimum and maximum allowable input.
 *
 * @tparam Inputs Number of inputs.
 * @param u Input vector to clamp.
 * @param umin The minimum input magnitude.
 * @param umax The maximum input magnitude.
 * @return Clamped input vector.
 */
template <int Inputs>
constexpr Vectord<Inputs> ClampInputMaxMagnitude(const Vectord<Inputs>& u,
                                                 const Vectord<Inputs>& umin,
                                                 const Vectord<Inputs>& umax) {
  Vectord<Inputs> result;
  for (int i = 0; i < Inputs; ++i) {
    result(i) = std::clamp(u(i), umin(i), umax(i));
  }
  return result;
}

/**
 * Renormalize all inputs if any exceeds the maximum magnitude. Useful for
 * systems such as differential drivetrains.
 *
 * @tparam Inputs      Number of inputs.
 * @param u            The input vector.
 * @param maxMagnitude The maximum magnitude any input can have.
 * @return The normalizedInput
 */
template <int Inputs>
Vectord<Inputs> DesaturateInputVector(const Vectord<Inputs>& u,
                                      double maxMagnitude) {
  double maxValue = u.template lpNorm<Eigen::Infinity>();

  if (maxValue > maxMagnitude) {
    return u * maxMagnitude / maxValue;
  }
  return u;
}
}  // namespace frc
