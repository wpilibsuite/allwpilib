// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <numbers>

#include "frc/EigenCore.h"
#include "frc/MathUtil.h"

namespace frc {

/**
 * Subtracts a and b while normalizing the resulting value in the selected row
 * as if it were an angle.
 *
 * @tparam States Number of states.
 * @param a A vector to subtract from.
 * @param b A vector to subtract with.
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
Vectord<States> AngleResidual(const Vectord<States>& a,
                              const Vectord<States>& b, int angleStateIdx) {
  Vectord<States> ret = a - b;
  ret[angleStateIdx] =
      AngleModulus(units::radian_t{ret[angleStateIdx]}).value();
  return ret;
}

/**
 * Returns a function that subtracts two vectors while normalizing the resulting
 * value in the selected row as if it were an angle.
 *
 * @tparam States Number of states.
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
std::function<Vectord<States>(const Vectord<States>&, const Vectord<States>&)>
AngleResidual(int angleStateIdx) {
  return [=](auto a, auto b) {
    return AngleResidual<States>(a, b, angleStateIdx);
  };
}

/**
 * Adds a and b while normalizing the resulting value in the selected row as an
 * angle.
 *
 * @tparam States Number of states.
 * @param a A vector to add with.
 * @param b A vector to add with.
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
Vectord<States> AngleAdd(const Vectord<States>& a, const Vectord<States>& b,
                         int angleStateIdx) {
  Vectord<States> ret = a + b;
  ret[angleStateIdx] =
      InputModulus(ret[angleStateIdx], -std::numbers::pi, std::numbers::pi);
  return ret;
}

/**
 * Returns a function that adds two vectors while normalizing the resulting
 * value in the selected row as an angle.
 *
 * @tparam States Number of states.
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
std::function<Vectord<States>(const Vectord<States>&, const Vectord<States>&)>
AngleAdd(int angleStateIdx) {
  return [=](auto a, auto b) { return AngleAdd<States>(a, b, angleStateIdx); };
}

/**
 * Computes the mean of sigmas with the weights Wm while computing a special
 * angle mean for a select row.
 *
 * @tparam CovDim Dimension of covariance of sigma points after passing through
 *                the transform.
 * @tparam States Number of states.
 * @param sigmas Sigma points.
 * @param Wm Weights for the mean.
 * @param angleStatesIdx The row containing the angles.
 */
template <int CovDim, int States>
Vectord<CovDim> AngleMean(const Matrixd<CovDim, 2 * States + 1>& sigmas,
                          const Vectord<2 * States + 1>& Wm,
                          int angleStatesIdx) {
  double sumSin = (sigmas.row(angleStatesIdx).unaryExpr([](auto it) {
                    return std::sin(it);
                  }) *
                   Wm)
                      .sum();
  double sumCos = (sigmas.row(angleStatesIdx).unaryExpr([](auto it) {
                    return std::cos(it);
                  }) *
                   Wm)
                      .sum();

  Vectord<CovDim> ret = sigmas * Wm;
  ret[angleStatesIdx] = std::atan2(sumSin, sumCos);
  return ret;
}

/**
 * Returns a function that computes the mean of sigmas with the weights Wm while
 * computing a special angle mean for a select row.
 *
 * @tparam CovDim Dimension of covariance of sigma points after passing through
 *                the transform.
 * @tparam States Number of states.
 * @param angleStateIdx The row containing the angles.
 */
template <int CovDim, int States>
std::function<Vectord<CovDim>(const Matrixd<CovDim, 2 * States + 1>&,
                              const Vectord<2 * States + 1>&)>
AngleMean(int angleStateIdx) {
  return [=](auto sigmas, auto Wm) {
    return AngleMean<CovDim, States>(sigmas, Wm, angleStateIdx);
  };
}

}  // namespace frc
