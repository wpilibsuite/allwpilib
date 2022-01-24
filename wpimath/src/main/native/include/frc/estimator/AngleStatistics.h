// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/numbers>

#include "Eigen/Core"
#include "frc/MathUtil.h"

namespace frc {

/**
 * Subtracts a and b while normalizing the resulting value in the selected row
 * as if it were an angle.
 *
 * @tparam States The number of states.
 * @param a A vector to subtract from.
 * @param b A vector to subtract with.
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
Eigen::Vector<double, States> AngleResidual(
    const Eigen::Vector<double, States>& a,
    const Eigen::Vector<double, States>& b, int angleStateIdx) {
  Eigen::Vector<double, States> ret = a - b;
  ret[angleStateIdx] =
      AngleModulus(units::radian_t{ret[angleStateIdx]}).value();
  return ret;
}

/**
 * Returns a function that subtracts two vectors while normalizing the resulting
 * value in the selected row as if it were an angle.
 *
 * @tparam States The number of states.
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
std::function<Eigen::Vector<double, States>(
    const Eigen::Vector<double, States>&, const Eigen::Vector<double, States>&)>
AngleResidual(int angleStateIdx) {
  return [=](auto a, auto b) {
    return AngleResidual<States>(a, b, angleStateIdx);
  };
}

/**
 * Adds a and b while normalizing the resulting value in the selected row as an
 * angle.
 *
 * @tparam States The number of states.
 * @param a A vector to add with.
 * @param b A vector to add with.
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
Eigen::Vector<double, States> AngleAdd(const Eigen::Vector<double, States>& a,
                                       const Eigen::Vector<double, States>& b,
                                       int angleStateIdx) {
  Eigen::Vector<double, States> ret = a + b;
  ret[angleStateIdx] =
      InputModulus(ret[angleStateIdx], -wpi::numbers::pi, wpi::numbers::pi);
  return ret;
}

/**
 * Returns a function that adds two vectors while normalizing the resulting
 * value in the selected row as an angle.
 *
 * @tparam States The number of states.
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
std::function<Eigen::Vector<double, States>(
    const Eigen::Vector<double, States>&, const Eigen::Vector<double, States>&)>
AngleAdd(int angleStateIdx) {
  return [=](auto a, auto b) { return AngleAdd<States>(a, b, angleStateIdx); };
}

/**
 * Computes the mean of sigmas with the weights Wm while computing a special
 * angle mean for a select row.
 *
 * @tparam CovDim Dimension of covariance of sigma points after passing through
 *                the transform.
 * @tparam States The number of states.
 * @param sigmas Sigma points.
 * @param Wm Weights for the mean.
 * @param angleStatesIdx The row containing the angles.
 */
template <int CovDim, int States>
Eigen::Vector<double, CovDim> AngleMean(
    const Eigen::Matrix<double, CovDim, 2 * States + 1>& sigmas,
    const Eigen::Vector<double, 2 * States + 1>& Wm, int angleStatesIdx) {
  double sumSin = sigmas.row(angleStatesIdx)
                      .unaryExpr([](auto it) { return std::sin(it); })
                      .sum();
  double sumCos = sigmas.row(angleStatesIdx)
                      .unaryExpr([](auto it) { return std::cos(it); })
                      .sum();

  Eigen::Vector<double, CovDim> ret = sigmas * Wm;
  ret[angleStatesIdx] = std::atan2(sumSin, sumCos);
  return ret;
}

/**
 * Returns a function that computes the mean of sigmas with the weights Wm while
 * computing a special angle mean for a select row.
 *
 * @tparam CovDim Dimension of covariance of sigma points after passing through
 *                the transform.
 * @tparam States The number of states.
 * @param angleStateIdx The row containing the angles.
 */
template <int CovDim, int States>
std::function<Eigen::Vector<double, CovDim>(
    const Eigen::Matrix<double, CovDim, 2 * States + 1>&,
    const Eigen::Vector<double, 2 * States + 1>&)>
AngleMean(int angleStateIdx) {
  return [=](auto sigmas, auto Wm) {
    return AngleMean<CovDim, States>(sigmas, Wm, angleStateIdx);
  };
}

}  // namespace frc
