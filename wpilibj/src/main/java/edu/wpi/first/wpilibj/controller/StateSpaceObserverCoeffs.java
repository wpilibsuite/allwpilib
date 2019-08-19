/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;

/**
 * A container for all the observer coefficients.
 */
@SuppressWarnings({"unused", "ClassTypeParameterName", "MemberName", "ParameterName", "LineLength"})
public final class StateSpaceObserverCoeffs<States extends Num, Inputs extends Num, Outputs extends Num> {
  /**
   * Estimator gain matrix.
   */
  private final Matrix<States, Outputs> K;

  /**
   * Construct the container for the observer coefficients.
   *
   * @param K The Kalman gain matrix.
   */
  public StateSpaceObserverCoeffs(Matrix<States, Outputs> K) {
    this.K = K;
  }

  public Matrix<States, Outputs> getK() {
    return K;
  }
}
