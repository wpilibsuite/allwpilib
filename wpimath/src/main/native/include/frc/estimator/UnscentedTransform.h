// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <tuple>

#include <Eigen/QR>

#include "frc/EigenCore.h"

namespace frc {

/**
 * Computes unscented transform of a set of sigma points and weights. CovDim
 * returns the mean and square-root covariance of the sigma points in a tuple.
 *
 * This works in conjunction with the UnscentedKalmanFilter class. For use with
 * square-root form UKFs.
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
 * @param squareRootR  Square-root of the noise covariance of the sigma points.
 *
 * @return Tuple of x, mean of sigma points; S, square-root covariance of
 * sigmas.
 */
template <int CovDim, int States>
std::tuple<Vectord<CovDim>, Matrixd<CovDim, CovDim>>
SquareRootUnscentedTransform(
    const Matrixd<CovDim, 2 * States + 1>& sigmas,
    const Vectord<2 * States + 1>& Wm, const Vectord<2 * States + 1>& Wc,
    std::function<Vectord<CovDim>(const Matrixd<CovDim, 2 * States + 1>&,
                                  const Vectord<2 * States + 1>&)>
        meanFunc,
    std::function<Vectord<CovDim>(const Vectord<CovDim>&,
                                  const Vectord<CovDim>&)>
        residualFunc,
    const Matrixd<CovDim, CovDim>& squareRootR) {
  // New mean is usually just the sum of the sigmas * weight:
  //       n
  // dot = Σ W[k] Xᵢ[k]
  //      k=1
  Vectord<CovDim> x = meanFunc(sigmas, Wm);

  Matrixd<CovDim, States * 2 + CovDim> Sbar;
  for (int i = 0; i < States * 2; i++) {
    Sbar.template block<CovDim, 1>(0, i) =
        std::sqrt(Wc[1]) *
        residualFunc(sigmas.template block<CovDim, 1>(0, 1 + i), x);
  }
  Sbar.template block<CovDim, CovDim>(0, States * 2) = squareRootR;

  // Merwe defines the QR decomposition as Aᵀ = QR
  Matrixd<CovDim, CovDim> S = Sbar.transpose()
                                  .householderQr()
                                  .matrixQR()
                                  .template block<CovDim, CovDim>(0, 0)
                                  .template triangularView<Eigen::Upper>();

  Eigen::internal::llt_inplace<double, Eigen::Upper>::rankUpdate(
      S, residualFunc(sigmas.template block<CovDim, 1>(0, 0), x), Wc[0]);

  return std::make_tuple(x, S);
}

}  // namespace frc
