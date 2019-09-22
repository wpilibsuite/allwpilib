/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>
#include <cmath>
#include <random>
#include <type_traits>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/QR>

namespace frc {
namespace detail {

template <typename Matrix, typename T, typename... Ts>
void CostMatrixImpl(Matrix& result, T elem, Ts... elems) {
  result(result.rows() - (sizeof...(Ts) + 1)) = 1.0 / std::pow(elem, 2);
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
}  // namespace detail

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
template <typename... Ts, typename = std::enable_if_t<
                              std::conjunction_v<std::is_same<double, Ts>...>>>
Eigen::Matrix<double, sizeof...(Ts), sizeof...(Ts)> MakeCostMatrix(
    Ts... costs) {
  Eigen::DiagonalMatrix<double, sizeof...(Ts)> result;
  detail::CostMatrixImpl(result.diagonal(), costs...);
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
Eigen::Matrix<double, sizeof...(Ts), sizeof...(Ts)> MakeCovMatrix(
    Ts... stdDevs) {
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
Eigen::Matrix<double, N, N> MakeCostMatrix(const std::array<double, N>& costs) {
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
Eigen::Matrix<double, N, N> MakeCovMatrix(
    const std::array<double, N>& stdDevs) {
  Eigen::DiagonalMatrix<double, N> result;
  auto& diag = result.diagonal();
  for (size_t i = 0; i < N; ++i) {
    diag(i) = std::pow(stdDevs[i], 2);
  }
  return result;
}

template <typename... Ts, typename = std::enable_if_t<
                              std::conjunction_v<std::is_same<double, Ts>...>>>
Eigen::Matrix<double, sizeof...(Ts), 1> MakeWhiteNoiseVector(Ts... stdDevs) {
  Eigen::Matrix<double, sizeof...(Ts), 1> result;
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
Eigen::Matrix<double, N, 1> MakeWhiteNoiseVector(
    const std::array<double, N>& stdDevs) {
  std::random_device rd;
  std::mt19937 gen{rd()};

  Eigen::Matrix<double, N, 1> result;
  for (int i = 0; i < N; ++i) {
    std::normal_distribution<> distr{0.0, stdDevs[i]};
    result(i, 0) = distr(gen);
  }
  return result;
}

/**
 * Returns true if (A, B) is a stabilizable pair.
 *
 * (A,B) is stabilizable if and only if the uncontrollable eigenvalues of A, if
 * any, have absolute values less than one, where an eigenvalue is
 * uncontrollable if rank(lambda * I - A, B) < n where n is number of states.
 *
 * @param A System matrix.
 * @param B Input matrix.
 */
template <int States, int Inputs>
bool IsStabilizable(const Eigen::Matrix<double, States, States>& A,
                    const Eigen::Matrix<double, States, Inputs>& B) {
  Eigen::EigenSolver<Eigen::Matrix<double, States, States>> es(A);

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
        qr(E);
    if (qr.rank() < States) {
      return false;
    }
  }
  return true;
}

}  // namespace frc
