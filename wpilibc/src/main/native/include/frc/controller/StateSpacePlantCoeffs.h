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
 * A container for all the state-space plant coefficients.
 */
template <int States, int Inputs, int Outputs>
struct StateSpacePlantCoeffs final {
  /**
   * System matrix.
   */
  const Eigen::Matrix<double, States, States> A;

  /**
   * Input matrix.
   */
  const Eigen::Matrix<double, States, Inputs> B;

  /**
   * Output matrix.
   */
  const Eigen::Matrix<double, Outputs, States> C;

  /**
   * Feedthrough matrix.
   */
  const Eigen::Matrix<double, Outputs, Inputs> D;

  /**
   * Construct the container for the state-space plant coefficients.
   *
   * @param A System matrix.
   * @param B Input matrix.
   * @param C Output matrix.
   * @param D Feedthrough matrix.
   */
  StateSpacePlantCoeffs(const Eigen::Matrix<double, States, States>& A,
                        const Eigen::Matrix<double, States, Inputs>& B,
                        const Eigen::Matrix<double, Outputs, States>& C,
                        const Eigen::Matrix<double, Outputs, Inputs>& D);
};

}  // namespace frc

#include "frc/controller/StateSpacePlantCoeffs.inc"
