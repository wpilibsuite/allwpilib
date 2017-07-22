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
 * A container for all the period-variant plant coefficients.
 */
template <int States, int Inputs, int Outputs>
struct PeriodVariantPlantCoeffs final {
  /**
   * Continuous system matrix.
   */
  const Eigen::Matrix<double, States, States> Acontinuous;

  /**
   * Continuous input matrix.
   */
  const Eigen::Matrix<double, States, Inputs> Bcontinuous;

  /**
   * Output matrix.
   */
  const Eigen::Matrix<double, Outputs, States> C;

  /**
   * Feedthrough matrix.
   */
  const Eigen::Matrix<double, Outputs, Inputs> D;

  /**
   * Construct the container for the period-variant plant coefficients.
   *
   * @param Acontinuous Continuous system matrix.
   * @param Bcontinuous Continuous input matrix.
   * @param C           Output matrix.
   * @param D           Feedthrough matrix.
   */
  PeriodVariantPlantCoeffs(
      const Eigen::Matrix<double, States, States>& Acontinuous,
      const Eigen::Matrix<double, States, Inputs>& Bcontinuous,
      const Eigen::Matrix<double, Outputs, States>& C,
      const Eigen::Matrix<double, Outputs, Inputs>& D);
};

}  // namespace frc

#include "frc/controller/PeriodVariantPlantCoeffs.inc"
