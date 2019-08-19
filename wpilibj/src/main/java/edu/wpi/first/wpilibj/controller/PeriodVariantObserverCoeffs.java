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
@SuppressWarnings({"ClassTypeParameterName", "MemberName", "ParameterName", "LineLength"})
public final class PeriodVariantObserverCoeffs<States extends Num, Inputs extends Num, Outputs extends Num> {
  /**
   * The continuous process noise covariance matrix.
   */
  private final Matrix<States, States> Qcontinuous;

  /**
   * The continuous measurement noise covariance matrix.
   */
  private final Matrix<Outputs, Outputs> Rcontinuous;

  /**
   * The steady-state error covariance matrix.
   */
  private final Matrix<States, States> PsteadyState;

  /**
   * Construct the container for the observer coefficients.
   *
   * @param Qcontinuous  The continuous process noise covariance matrix.
   * @param Rcontinuous  The continuous measurement noise covariance matrix.
   * @param PsteadyState The steady-state error covariance matrix.
   */
  public PeriodVariantObserverCoeffs(Matrix<States, States> Qcontinuous,
                                     Matrix<Outputs, Outputs> Rcontinuous,
                                     Matrix<States, States> PsteadyState) {
    this.Qcontinuous = Qcontinuous;
    this.Rcontinuous = Rcontinuous;
    this.PsteadyState = PsteadyState;
  }

  public Matrix<States, States> getQcontinuous() {
    return Qcontinuous;
  }

  public Matrix<Outputs, Outputs> getRcontinuous() {
    return Rcontinuous;
  }

  public Matrix<States, States> getPsteadyState() {
    return PsteadyState;
  }
}
