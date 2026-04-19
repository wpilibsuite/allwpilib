// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.util;

import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.numbers.N1;

/** State-space utilities. */
public final class StateSpaceUtil {
  private StateSpaceUtil() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Creates a cost matrix from the given vector for use with LQR.
   *
   * <p>The cost matrix is constructed using Bryson's rule. The inverse square of each tolerance is
   * placed on the cost matrix diagonal. If a tolerance is infinity, its cost matrix entry is set to
   * zero.
   *
   * @param <Elements> Nat representing the number of system states or inputs.
   * @param tolerances An array. For a Q matrix, its elements are the maximum allowed excursions of
   *     the states from the reference. For an R matrix, its elements are the maximum allowed
   *     excursions of the control inputs from no actuation.
   * @return State excursion or control effort cost matrix.
   */
  public static <Elements extends Num> Matrix<Elements, Elements> costMatrix(
      Matrix<Elements, N1> tolerances) {
    Matrix<Elements, Elements> result =
        new Matrix<>(new SimpleMatrix(tolerances.getNumRows(), tolerances.getNumRows()));
    result.fill(0.0);

    for (int i = 0; i < tolerances.getNumRows(); i++) {
      if (tolerances.get(i, 0) == Double.POSITIVE_INFINITY) {
        result.set(i, i, 0.0);
      } else {
        result.set(i, i, 1.0 / Math.pow(tolerances.get(i, 0), 2));
      }
    }

    return result;
  }

  /**
   * Creates a covariance matrix from the given vector for use with Kalman filters.
   *
   * <p>Each element is squared and placed on the covariance matrix diagonal.
   *
   * @param <States> Num representing the states of the system.
   * @param states A Nat representing the states of the system.
   * @param stdDevs For a Q matrix, its elements are the standard deviations of each state from how
   *     the model behaves. For an R matrix, its elements are the standard deviations for each
   *     output measurement.
   * @return Process noise or measurement noise covariance matrix.
   */
  public static <States extends Num> Matrix<States, States> covarianceMatrix(
      Nat<States> states, Matrix<States, N1> stdDevs) {
    var result = new Matrix<>(states, states);
    for (int i = 0; i < states.getNum(); i++) {
      result.set(i, i, Math.pow(stdDevs.get(i, 0), 2));
    }
    return result;
  }

  /**
   * Renormalize all inputs if any exceeds the maximum magnitude. Useful for systems such as
   * differential drivetrains.
   *
   * @param u The input vector.
   * @param maxMagnitude The maximum magnitude any input can have.
   * @param <I> Number of inputs.
   * @return The normalizedInput
   */
  public static <I extends Num> Matrix<I, N1> desaturateInputVector(
      Matrix<I, N1> u, double maxMagnitude) {
    return u.times(Math.min(1.0, maxMagnitude / u.maxAbs()));
  }
}
