/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <tuple>

#include <Eigen/Core>

namespace frc {

/**
 * Computes unscented transform of a set of sigma points and weights. CovDimurns
 * the mean and covariance in a tuple.
 *
 * This works in conjunction with the UnscentedKalmanFilter class.
 *
 * @tparam States  Number of states.
 * @tparam CovDim  Dimension of covariance of sigma points after passing through
 *                 the transform.
 * @param sigmas   List of sigma points.
 * @param Wm       Weights for the mean.
 * @param Wc       Weights for the covariance.
 *
 * @return Tuple of x, mean of sigma points; P, covariance of sigma points after
 *         passing through the transform.
 */
template <int States, int CovDim>
std::tuple<Eigen::Matrix<double, CovDim, 1>,
           Eigen::Matrix<double, CovDim, CovDim>>
UnscentedTransform(const Eigen::Matrix<double, 2 * States + 1, CovDim>& sigmas,
                   const Eigen::Matrix<double, 1, 2 * States + 1>& Wm,
                   const Eigen::Matrix<double, 1, 2 * States + 1>& Wc) {
  // New mean is just the sum of the sigmas * weight
  // dot = \Sigma^n_1 (W[k]*Xi[k])
  Eigen::Matrix<double, 1, CovDim> x = Wm * sigmas;

  // New covariance is the sum of the outer product of the residuals times the
  // weights
  Eigen::Matrix<double, 2 * States + 1, CovDim> y;
  for (int i = 0; i < 2 * States + 1; ++i) {
    y.template block<1, CovDim>(i, 0) =
        sigmas.template block<1, CovDim>(i, 0) - x;
  }
  Eigen::Matrix<double, CovDim, CovDim> P =
      y.transpose() * Eigen::DiagonalMatrix<double, 2 * States + 1>(Wc) * y;

  return std::make_tuple(x.transpose(), P);
}

}  // namespace frc
