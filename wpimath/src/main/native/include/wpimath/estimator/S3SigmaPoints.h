// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/array.h>

#include "wpimath/EigenCore.h"
#include "wpimath/estimator/SigmaPoints.h"

namespace wpimath {

/**
 * Generates sigma points and weights according to Papakonstantinou's paper[1]
 * for the UnscentedKalmanFilter class.
 *
 * It parameterizes the sigma points using alpha and beta terms. Unless you know
 * better, this should be your default choice due to its high accuracy and
 * performance.
 *
 * [1] K. Papakonstantinou "A Scaled Spherical Simplex Filter (S3F) with a
 * decreased n + 2 sigma points set size and equivalent 2n + 1 Unscented Kalman
 * Filter (UKF) accuracy"
 *
 * @tparam States The dimenstionality of the state. States + 2 weights will be
 * generated.
 */
template <int States>
class S3SigmaPoints {
 public:
  static constexpr int NumSigmas = States + 2;

  /**
   * Constructs a generator for Papakonstantinou sigma points.
   *
   * @param alpha Determines the spread of the sigma points around the mean.
   * Usually a small positive value (1e-3).
   * @param beta Incorporates prior knowledge of the distribution of the mean.
   * For Gaussian distributions, beta = 2 is optimal.
   */
  explicit S3SigmaPoints(double alpha = 1e-3, double beta = 2)
      : m_alpha{alpha} {
    ComputeWeights(beta);
  }

  /**
   * Computes the sigma points for an unscented Kalman filter given the mean (x)
   * and square-root covariance (S) of the filter.
   *
   * @param x An array of the means.
   * @param S Square-root covariance of the filter.
   *
   * @return Two dimensional array of sigma points. Each column contains all of
   * the sigmas for one dimension in the problem space. Ordered by Xi_0,
   * Xi_{1..n+1}.
   */
  Matrixd<States, NumSigmas> SquareRootSigmaPoints(
      const Vectord<States>& x, const Matrixd<States, States>& S) const {
    // table (1), equation (12)
    wpi::array<double, States> q{wpi::empty_array};
    for (size_t t = 1; t <= States; ++t) {
      q[t - 1] = m_alpha * std::sqrt(static_cast<double>(t * (States + 1)) /
                                     static_cast<double>(t + 1));
    }

    Matrixd<States, NumSigmas> C;
    C.template block<States, 1>(0, 0) = Vectord<States>::Constant(0.0);
    for (int col = 1; col < NumSigmas; ++col) {
      for (int row = 0; row < States; ++row) {
        if (row < col - 2) {
          C(row, col) = 0.0;
        } else if (row == col - 2) {
          C(row, col) = q[row];
        } else {
          C(row, col) = -q[row] / (row + 1);
        }
      }
    }

    Matrixd<States, NumSigmas> sigmas;
    for (int col = 0; col < NumSigmas; ++col) {
      sigmas.col(col) = x + S * C.col(col);
    }

    return sigmas;
  }

  /**
   * Returns the weight for each sigma point for the mean.
   */
  const Vectord<NumSigmas>& Wm() const { return m_Wm; }

  /**
   * Returns an element of the weight for each sigma point for the mean.
   *
   * @param i Element of vector to return.
   */
  double Wm(int i) const { return m_Wm(i, 0); }

  /**
   * Returns the weight for each sigma point for the covariance.
   */
  const Vectord<NumSigmas>& Wc() const { return m_Wc; }

  /**
   * Returns an element of the weight for each sigma point for the covariance.
   *
   * @param i Element of vector to return.
   */
  double Wc(int i) const { return m_Wc(i, 0); }

 private:
  Vectord<NumSigmas> m_Wm;
  Vectord<NumSigmas> m_Wc;
  double m_alpha;

  /**
   * Computes the weights for the scaled unscented Kalman filter.
   *
   * @param beta Incorporates prior knowledge of the distribution of the mean.
   */
  void ComputeWeights(double beta) {
    double alpha_sq = m_alpha * m_alpha;

    double c = 1.0 / (alpha_sq * (States + 1));
    m_Wm = Vectord<NumSigmas>::Constant(c);
    m_Wc = Vectord<NumSigmas>::Constant(c);

    m_Wm(0) = 1.0 - 1.0 / alpha_sq;
    m_Wc(0) = 1.0 - 1.0 / alpha_sq + (1 - alpha_sq + beta);
  }
};

}  // namespace wpimath
