/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/math>

#include "Eigen/Core"

namespace frc {
namespace {
double NormalizeAngle(double angle) {
  static constexpr double tau = 2 * wpi::math::pi;

  angle -= std::floor(angle / tau) * tau;
  if (angle > wpi::math::pi) angle -= tau;
  return angle;
}
}  // namespace

/**
 * Subtracts a and b while normalizing the resulting value in the selected row
 * as if it were an angle.
 *
 * @param a A vector to subtract from.
 * @param b A vector to subtract with.
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
Eigen::Matrix<double, States, 1> AngleResidual(
    const Eigen::Matrix<double, States, 1>& a,
    const Eigen::Matrix<double, States, 1>& b, int angleStateIdx) {
  Eigen::Matrix<double, States, 1> ret = a - b;
  ret[angleStateIdx] = NormalizeAngle(ret[angleStateIdx]);
  return ret;
}

/**
 * Returns a function that subtracts two vectors while normalizing the resulting
 * value in the selected row as if it were an angle.
 *
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
std::function<
    Eigen::Matrix<double, States, 1>(const Eigen::Matrix<double, States, 1>&,
                                     const Eigen::Matrix<double, States, 1>&)>
AngleResidual(int angleStateIdx) {
  return [=](auto a, auto b) {
    return AngleResidual<States>(a, b, angleStateIdx);
  };
}

/**
 * Adds a and b while normalizing the resulting value in the selected row as an
 * angle.
 *
 * @param a A vector to add with.
 * @param b A vector to add with.
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
Eigen::Matrix<double, States, 1> AngleAdd(
    const Eigen::Matrix<double, States, 1>& a,
    const Eigen::Matrix<double, States, 1>& b, int angleStateIdx) {
  Eigen::Matrix<double, States, 1> ret = a + b;
  ret[angleStateIdx] = NormalizeAngle(ret[angleStateIdx]);
  return ret;
}

/**
 * Returns a function that adds two vectors while normalizing the resulting
 * value in the selected row as an angle.
 *
 * @param angleStateIdx The row containing angles to be normalized.
 */
template <int States>
std::function<
    Eigen::Matrix<double, States, 1>(const Eigen::Matrix<double, States, 1>&,
                                     const Eigen::Matrix<double, States, 1>&)>
AngleAdd(int angleStateIdx) {
  return [=](auto a, auto b) { return AngleAdd<States>(a, b, angleStateIdx); };
}

/**
 * Computes the mean of sigmas with the weights Wm while computing a special
 * angle mean for a select row.
 *
 * @param sigmas Sigma points.
 * @param Wm Weights for the mean.
 * @param angleStateIdx The row containing the angles.
 */
template <int CovDim, int States>
Eigen::Matrix<double, CovDim, 1> AngleMean(
    const Eigen::Matrix<double, CovDim, 2 * States + 1>& sigmas,
    const Eigen::Matrix<double, 2 * States + 1, 1>& Wm, int angleStatesIdx) {
  double sumSin = sigmas.row(angleStatesIdx)
                      .unaryExpr([](auto it) { return std::sin(it); })
                      .sum();
  double sumCos = sigmas.row(angleStatesIdx)
                      .unaryExpr([](auto it) { return std::cos(it); })
                      .sum();

  Eigen::Matrix<double, CovDim, 1> ret = sigmas * Wm;
  ret[angleStatesIdx] = std::atan2(sumSin, sumCos);
  return ret;
}

/**
 * Returns a function that computes the mean of sigmas with the weights Wm while
 * computing a special angle mean for a select row.
 *
 * @param angleStateIdx The row containing the angles.
 */
template <int CovDim, int States>
std::function<Eigen::Matrix<double, CovDim, 1>(
    const Eigen::Matrix<double, CovDim, 2 * States + 1>&,
    const Eigen::Matrix<double, 2 * States + 1, 1>&)>
AngleMean(int angleStateIdx) {
  return [=](auto sigmas, auto Wm) {
    return AngleMean<CovDim, States>(sigmas, Wm, angleStateIdx);
  };
}
}  // namespace frc
