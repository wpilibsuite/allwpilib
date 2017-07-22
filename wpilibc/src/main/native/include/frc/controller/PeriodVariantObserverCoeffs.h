/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Eigen/Core>

namespace frc {

/**
 * A container for all the observer coefficients.
 */
template <int States, int Inputs, int Outputs>
struct PeriodVariantObserverCoeffs final {
  /**
   * The continuous process noise covariance matrix.
   */
  Eigen::Matrix<double, States, States> Qcontinuous;

  /**
   * The continuous measurement noise covariance matrix.
   */
  Eigen::Matrix<double, Outputs, Outputs> Rcontinuous;

  /**
   * The steady-state error covariance matrix.
   */
  Eigen::Matrix<double, States, States> PsteadyState;

  /**
   * Construct the container for the observer coefficients.
   *
   * @param Qcontinuous The continuous process noise covariance matrix.
   * @param Rcontinuous The continuous measurement noise covariance matrix.
   * @param PsteadyState The steady-state error covariance matrix.
   */
  PeriodVariantObserverCoeffs(
      const Eigen::Matrix<double, States, States>& Qcontinuous,
      const Eigen::Matrix<double, Outputs, Outputs>& Rcontinuous,
      const Eigen::Matrix<double, States, States>& PsteadyState);
};

}  // namespace frc

#include "frc/controller/PeriodVariantObserverCoeffs.inc"
