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
 * A container for all the state-space controller coefficients.
 */
template <int States, int Inputs, int Outputs>
struct StateSpaceControllerCoeffs final {
  /**
   * Controller gain matrix.
   */
  const Eigen::Matrix<double, Inputs, States> K;

  /**
   * Controller feedforward gain matrix.
   */
  const Eigen::Matrix<double, Inputs, States> Kff;

  /**
   * Minimum control input.
   */
  const Eigen::Matrix<double, Inputs, 1> Umin;

  /**
   * Maximum control input.
   */
  const Eigen::Matrix<double, Inputs, 1> Umax;

  /**
   * Construct the container for the controller coefficients.
   *
   * @param K    Controller gain matrix.
   * @param Kff  Controller feedforward gain matrix.
   * @param Umin Minimum control input.
   * @param Umax Maximum control input.
   */
  StateSpaceControllerCoeffs(const Eigen::Matrix<double, Inputs, States>& K,
                             const Eigen::Matrix<double, Inputs, States>& Kff,
                             const Eigen::Matrix<double, Inputs, 1>& Umin,
                             const Eigen::Matrix<double, Inputs, 1>& Umax);
};

}  // namespace frc

#include "frc/controller/StateSpaceControllerCoeffs.inc"
