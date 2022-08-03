// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <type_traits>

#include <wpi/SymbolExports.h>
#include <wpi/deprecated.h>

#include "Eigen/Eigenvalues"
#include "Eigen/QR"
#include "frc/EigenCore.h"
#include "frc/geometry/Pose2d.h"

namespace frc {
namespace detail {

template <int Rows, int Cols, typename Matrix, typename T, typename... Ts>
void MatrixImpl(Matrix& result, T elem, Ts... elems) {
  constexpr int count = Rows * Cols - (sizeof...(Ts) + 1);

  result(count / Cols, count % Cols) = elem;
  if constexpr (sizeof...(Ts) > 0) {
    MatrixImpl<Rows, Cols>(result, elems...);
  }
}

template <typename Matrix, typename T, typename... Ts>
void CostMatrixImpl(Matrix& result, T elem, Ts... elems) {
  if (elem == std::numeric_limits<double>::infinity()) {
    result(result.rows() - (sizeof...(Ts) + 1)) = 0.0;
  } else {
    result(result.rows() - (sizeof...(Ts) + 1)) = 1.0 / std::pow(elem, 2);
  }
  if constexpr (sizeof...(Ts) > 0) {
    CostMatrixImpl(result, elems...);
  }
}

template <typename Matrix, typename T, typename... Ts>
void CovMatrixImpl(Matrix& result, T elem, Ts... elems) {
  result(result.rows() - (sizeof...(Ts) + 1)) = std::pow(elem, 2);
  if constexpr (sizeof...(Ts) > 0) {
    CovMatrixImpl(result, elems...);
  }
}

template <typename Matrix, typename T, typename... Ts>
void WhiteNoiseVectorImpl(Matrix& result, T elem, Ts... elems) {
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::normal_distribution<> distr{0.0, elem};

  result(result.rows() - (sizeof...(Ts) + 1)) = distr(gen);
  if constexpr (sizeof...(Ts) > 0) {
    WhiteNoiseVectorImpl(result, elems...);
  }
}

template <int States, int Inputs>
bool IsStabilizableImpl(const Matrixd<States, States>& A,
                        const Matrixd<States, Inputs>& B) {
  Eigen::EigenSolver<Matrixd<States, States>> es{A};

  for (int i = 0; i < States; ++i) {
    if (es.eigenvalues()[i].real() * es.eigenvalues()[i].real() +
            es.eigenvalues()[i].imag() * es.eigenvalues()[i].imag() <
        1) {
      continue;
    }

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
  }
  return true;
}

}  // namespace detail

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
template <typename... Ts, typename = std::enable_if_t<
                              std::conjunction_v<std::is_same<double, Ts>...>>>
Matrixd<sizeof...(Ts), sizeof...(Ts)> MakeCostMatrix(Ts... tolerances) {
  Eigen::DiagonalMatrix<double, sizeof...(Ts)> result;
  detail::CostMatrixImpl(result.diagonal(), tolerances...);
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
template <typename... Ts, typename = std::enable_if_t<
                              std::conjunction_v<std::is_same<double, Ts>...>>>
Matrixd<sizeof...(Ts), sizeof...(Ts)> MakeCovMatrix(Ts... stdDevs) {
  Eigen::DiagonalMatrix<double, sizeof...(Ts)> result;
  detail::CovMatrixImpl(result.diagonal(), stdDevs...);
  return result;
}

/**
 * Creates a cost matrix from the given vector for use with LQR.
 *
 * The cost matrix is constructed using Bryson's rule. The inverse square of
 * each element in the input is taken and placed on the cost matrix diagonal.
 *
 * @param costs An array. For a Q matrix, its elements are the maximum allowed
 *              excursions of the states from the reference. For an R matrix,
 *              its elements are the maximum allowed excursions of the control
 *              inputs from no actuation.
 * @return State excursion or control effort cost matrix.
 */
template <size_t N>
Matrixd<N, N> MakeCostMatrix(const std::array<double, N>& costs) {
  Eigen::DiagonalMatrix<double, N> result;
  auto& diag = result.diagonal();
  for (size_t i = 0; i < N; ++i) {
    diag(i) = 1.0 / std::pow(costs[i], 2);
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
Matrixd<N, N> MakeCovMatrix(const std::array<double, N>& stdDevs) {
  Eigen::DiagonalMatrix<double, N> result;
  auto& diag = result.diagonal();
  for (size_t i = 0; i < N; ++i) {
    diag(i) = std::pow(stdDevs[i], 2);
  }
  return result;
}

template <typename... Ts, typename = std::enable_if_t<
                              std::conjunction_v<std::is_same<double, Ts>...>>>
Matrixd<sizeof...(Ts), 1> MakeWhiteNoiseVector(Ts... stdDevs) {
  Matrixd<sizeof...(Ts), 1> result;
  detail::WhiteNoiseVectorImpl(result, stdDevs...);
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
  for (int i = 0; i < N; ++i) {
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
 */
WPILIB_DLLEXPORT
Vectord<3> PoseTo3dVector(const Pose2d& pose);

/**
 * Converts a Pose2d into a vector of [x, y, std::cos(theta), std::sin(theta)].
 *
 * @param pose The pose that is being represented.
 *
 * @return The vector.
 */
WPILIB_DLLEXPORT
Vectord<4> PoseTo4dVector(const Pose2d& pose);

/**
 * Returns true if (A, B) is a stabilizable pair.
 *
 * (A, B) is stabilizable if and only if the uncontrollable eigenvalues of A, if
 * any, have absolute values less than one, where an eigenvalue is
 * uncontrollable if rank(λI - A, B) < n where n is the number of states.
 *
 * @tparam States The number of states.
 * @tparam Inputs The number of inputs.
 * @param A System matrix.
 * @param B Input matrix.
 */
template <int States, int Inputs>
bool IsStabilizable(const Matrixd<States, States>& A,
                    const Matrixd<States, Inputs>& B) {
  return detail::IsStabilizableImpl<States, Inputs>(A, B);
}

/**
 * Returns true if (A, C) is a detectable pair.
 *
 * (A, C) is detectable if and only if the unobservable eigenvalues of A, if
 * any, have absolute values less than one, where an eigenvalue is unobservable
 * if rank(λI - A; C) < n where n is the number of states.
 *
 * @tparam States The number of states.
 * @tparam Outputs The number of outputs.
 * @param A System matrix.
 * @param C Output matrix.
 */
template <int States, int Outputs>
bool IsDetectable(const Matrixd<States, States>& A,
                  const Matrixd<Outputs, States>& C) {
  return detail::IsStabilizableImpl<States, Outputs>(A.transpose(),
                                                     C.transpose());
}

// Template specializations are used here to make common state-input pairs
// compile faster.
template <>
WPILIB_DLLEXPORT bool IsStabilizable<1, 1>(const Matrixd<1, 1>& A,
                                           const Matrixd<1, 1>& B);

// Template specializations are used here to make common state-input pairs
// compile faster.
template <>
WPILIB_DLLEXPORT bool IsStabilizable<2, 1>(const Matrixd<2, 2>& A,
                                           const Matrixd<2, 1>& B);

/**
 * Converts a Pose2d into a vector of [x, y, theta].
 *
 * @param pose The pose that is being represented.
 *
 * @return The vector.
 */
WPILIB_DLLEXPORT
Vectord<3> PoseToVector(const Pose2d& pose);

/**
 * Clamps input vector between system's minimum and maximum allowable input.
 *
 * @tparam Inputs The number of inputs.
 * @param u Input vector to clamp.
 * @param umin The minimum input magnitude.
 * @param umax The maximum input magnitude.
 * @return Clamped input vector.
 */
template <int Inputs>
Vectord<Inputs> ClampInputMaxMagnitude(const Vectord<Inputs>& u,
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
 * @tparam Inputs      The number of inputs.
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
