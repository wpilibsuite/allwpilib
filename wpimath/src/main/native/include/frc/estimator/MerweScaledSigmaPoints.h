// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>

#include "Eigen/Core"
#include "Eigen/src/Cholesky/LLT.h"

namespace frc {

/**
 * Generates sigma points and weights according to Van der Merwe's 2004
 * dissertation[1] for the UnscentedKalmanFilter class.
 *
 * It parametrizes the sigma points using alpha, beta, kappa terms, and is the
 * version seen in most publications. Unless you know better, this should be
 * your default choice.
 *
 * @tparam States The dimensionality of the state. 2*States+1 weights will be
 *                generated.
 *
 * [1] R. Van der Merwe "Sigma-Point Kalman Filters for Probabilitic
 *     Inference in Dynamic State-Space Models" (Doctoral dissertation)
 */
template <int States>
class MerweScaledSigmaPoints {
 public:
  /**
   * Constructs a generator for Van der Merwe scaled sigma points.
   *
   * @param alpha Determines the spread of the sigma points around the mean.
   *              Usually a small positive value (1e-3).
   * @param beta Incorporates prior knowledge of the distribution of the mean.
   *             For Gaussian distributions, beta = 2 is optimal.
   * @param kappa Secondary scaling parameter usually set to 0 or 3 - States.
   */
  explicit MerweScaledSigmaPoints(double alpha = 1e-3, double beta = 2,
                                  int kappa = 3 - States) {
    m_alpha = alpha;
    m_kappa = kappa;

    ComputeWeights(beta);
  }

  /**
   * Returns number of sigma points for each variable in the state x.
   */
  int NumSigmas() { return 2 * States + 1; }

  /**
   * Computes the sigma points for an unscented Kalman filter given the mean
   * (x) and covariance(P) of the filter.
   *
   * @param x An array of the means.
   * @param P Covariance of the filter.
   *
   * @return Two dimensional array of sigma points. Each column contains all of
   *         the sigmas for one dimension in the problem space. Ordered by
   *         Xi_0, Xi_{1..n}, Xi_{n+1..2n}.
   *
   */
  Eigen::Matrix<double, States, 2 * States + 1> SigmaPoints(
      const Eigen::Matrix<double, States, 1>& x,
      const Eigen::Matrix<double, States, States>& P) {
    double lambda = std::pow(m_alpha, 2) * (States + m_kappa) - States;
    Eigen::Matrix<double, States, States> U =
        ((lambda + States) * P).llt().matrixL();

    Eigen::Matrix<double, States, 2 * States + 1> sigmas;
    sigmas.template block<States, 1>(0, 0) = x;
    for (int k = 0; k < States; ++k) {
      sigmas.template block<States, 1>(0, k + 1) =
          x + U.template block<States, 1>(0, k);
      sigmas.template block<States, 1>(0, States + k + 1) =
          x - U.template block<States, 1>(0, k);
    }

    return sigmas;
  }

  /**
   * Returns the weight for each sigma point for the mean.
   */
  const Eigen::Matrix<double, 2 * States + 1, 1>& Wm() const { return m_Wm; }

  /**
   * Returns an element of the weight for each sigma point for the mean.
   *
   * @param i Element of vector to return.
   */
  double Wm(int i) const { return m_Wm(i, 0); }

  /**
   * Returns the weight for each sigma point for the covariance.
   */
  const Eigen::Matrix<double, 2 * States + 1, 1>& Wc() const { return m_Wc; }

  /**
   * Returns an element of the weight for each sigma point for the covariance.
   *
   * @param i Element of vector to return.
   */
  double Wc(int i) const { return m_Wc(i, 0); }

 private:
  Eigen::Matrix<double, 2 * States + 1, 1> m_Wm;
  Eigen::Matrix<double, 2 * States + 1, 1> m_Wc;
  double m_alpha;
  int m_kappa;

  /**
   * Computes the weights for the scaled unscented Kalman filter.
   *
   * @param beta Incorporates prior knowledge of the distribution of the mean.
   */
  void ComputeWeights(double beta) {
    double lambda = std::pow(m_alpha, 2) * (States + m_kappa) - States;

    double c = 0.5 / (States + lambda);
    m_Wm = Eigen::Matrix<double, 2 * States + 1, 1>::Constant(c);
    m_Wc = Eigen::Matrix<double, 2 * States + 1, 1>::Constant(c);

    m_Wm(0) = lambda / (States + lambda);
    m_Wc(0) = lambda / (States + lambda) + (1 - std::pow(m_alpha, 2) + beta);
  }
};

}  // namespace frc
