// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <tuple>

#include "Eigen/Core"

namespace frc {

/**
 * Computes unscented transform of a set of sigma points and weights. CovDim
 * returns the mean and covariance in a tuple.
 *
 * This works in conjunction with the UnscentedKalmanFilter class.
 *
 * @tparam CovDim      Dimension of covariance of sigma points after passing
 *                     through the transform.
 * @tparam States      Number of states.
 * @param sigmas       List of sigma points.
 * @param Wm           Weights for the mean.
 * @param Wc           Weights for the covariance.
 * @param meanFunc     A function that computes the mean of 2 * States + 1 state
 *                     vectors using a given set of weights.
 * @param residualFunc A function that computes the residual of two state
 *                     vectors (i.e. it subtracts them.)
 *
 * @return Tuple of x, mean of sigma points; P, covariance of sigma points after
 *         passing through the transform.
 */
template <int CovDim, int States>
std::tuple<Eigen::Vector<double, CovDim>, Eigen::Matrix<double, CovDim, CovDim>>
UnscentedTransform(const Eigen::Matrix<double, CovDim, 2 * States + 1>& sigmas,
                   const Eigen::Vector<double, 2 * States + 1>& Wm,
                   const Eigen::Vector<double, 2 * States + 1>& Wc,
                   std::function<Eigen::Vector<double, CovDim>(
                       const Eigen::Matrix<double, CovDim, 2 * States + 1>&,
                       const Eigen::Vector<double, 2 * States + 1>&)>
                       meanFunc,
                   std::function<Eigen::Vector<double, CovDim>(
                       const Eigen::Vector<double, CovDim>&,
                       const Eigen::Vector<double, CovDim>&)>
                       residualFunc) {
  // New mean is usually just the sum of the sigmas * weight:
  //       n
  // dot = Σ W[k] Xᵢ[k]
  //      k=1
  Eigen::Vector<double, CovDim> x = meanFunc(sigmas, Wm);

  // New covariance is the sum of the outer product of the residuals times the
  // weights
  Eigen::Matrix<double, CovDim, 2 * States + 1> y;
  for (int i = 0; i < 2 * States + 1; ++i) {
    // y[:, i] = sigmas[:, i] - x
    y.template block<CovDim, 1>(0, i) =
        residualFunc(sigmas.template block<CovDim, 1>(0, i), x);
  }
  Eigen::Matrix<double, CovDim, CovDim> P =
      y * Eigen::DiagonalMatrix<double, 2 * States + 1>(Wc) * y.transpose();

  return std::make_tuple(x, P);
}

}  // namespace frc
