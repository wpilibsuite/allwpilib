// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <tuple>

#include <Eigen/QR>

#include "wpi/math/linalg/EigenCore.hpp"

namespace wpi::math {

/**
 * Computes unscented transform of a set of sigma points and weights. CovDim
 * returns the mean and square-root covariance of the sigma points in a tuple.
 *
 * This works in conjunction with the UnscentedKalmanFilter class. For use with
 * square-root form UKFs.
 *
 * @tparam CovDim      Dimension of covariance of sigma points after passing
 *                     through the transform.
 * @tparam NumSigmas   Number of sigma points.
 * @param sigmas       List of sigma points.
 * @param Wm           Weights for the mean.
 * @param Wc           Weights for the covariance.
 * @param meanFunc     A function that computes the mean of NumSigmas state
 *                     vectors using a given set of weights.
 * @param residualFunc A function that computes the residual of two state
 *                     vectors (i.e. it subtracts them.)
 * @param squareRootR  Square-root of the noise covariance of the sigma points.
 *
 * @return Tuple of x, mean of sigma points; S, square-root covariance of
 * sigmas.
 */
template <int CovDim, int NumSigmas>
std::tuple<Vectord<CovDim>, Matrixd<CovDim, CovDim>>
SquareRootUnscentedTransform(
    const Matrixd<CovDim, NumSigmas>& sigmas, const Vectord<NumSigmas>& Wm,
    const Vectord<NumSigmas>& Wc,
    std::function<Vectord<CovDim>(const Matrixd<CovDim, NumSigmas>&,
                                  const Vectord<NumSigmas>&)>
        meanFunc,
    std::function<Vectord<CovDim>(const Vectord<CovDim>&,
                                  const Vectord<CovDim>&)>
        residualFunc,
    const Matrixd<CovDim, CovDim>& squareRootR) {
  // New mean is usually just the sum of the sigmas * weights:
  //
  //      2n
  //   x̂ = Σ Wᵢ⁽ᵐ⁾𝒳ᵢ
  //      i=0
  //
  // equations (19) and (23) in the paper show this,
  // but we allow a custom function, usually for angle wrapping
  Vectord<CovDim> x = meanFunc(sigmas, Wm);

  // Form an intermediate matrix S⁻ as:
  //
  //   [√{W₁⁽ᶜ⁾}(𝒳_{1:2L} - x̂) √{Rᵛ}]
  //
  // the part of equations (20) and (24) within the "qr{}"
  //
  // Note that we allow a custom function instead of the difference to allow
  // angle wrapping. Furthermore, we allow an arbitrary number of sigma points
  // to support similar methods such as the Scaled Spherical Simplex Filter
  // (S3F).
  Matrixd<CovDim, NumSigmas - 1 + CovDim> Sbar;
  for (int i = 0; i < NumSigmas - 1; i++) {
    Sbar.template block<CovDim, 1>(0, i) =
        std::sqrt(Wc[1]) *
        residualFunc(sigmas.template block<CovDim, 1>(0, 1 + i), x);
  }
  Sbar.template block<CovDim, CovDim>(0, NumSigmas - 1) = squareRootR;

  // Compute the square-root covariance of the sigma points.
  //
  // We transpose S⁻ first because we formed it by horizontally
  // concatenating each part; it should be vertical so we can take
  // the QR decomposition as defined in the "QR Decomposition" passage
  // of section 3. "EFFICIENT SQUARE-ROOT IMPLEMENTATION"
  //
  // The resulting matrix R is the square-root covariance S, but it
  // is upper triangular, so we need to transpose it.
  //
  // equations (20) and (24)
  Matrixd<CovDim, CovDim> S = Sbar.transpose()
                                  .householderQr()
                                  .matrixQR()
                                  .template block<CovDim, CovDim>(0, 0)
                                  .template triangularView<Eigen::Upper>()
                                  .transpose();

  // Update or downdate the square-root covariance with (𝒳₀-x̂)
  // depending on whether its weight (W₀⁽ᶜ⁾) is positive or negative.
  //
  // equations (21) and (25)
  Eigen::internal::llt_inplace<double, Eigen::Lower>::rankUpdate(
      S, residualFunc(sigmas.template block<CovDim, 1>(0, 0), x), Wc[0]);

  return std::make_tuple(x, S);
}

}  // namespace wpi::math
