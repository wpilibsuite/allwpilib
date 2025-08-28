// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.util.Num;
import org.wpilib.math.numbers.N1;

/**
 * A sigma points generator for the UnscentedKalmanFilter class.
 *
 * @param <States> The dimensionality of the state.
 */
public interface SigmaPoints<States extends Num> {
  /**
   * Returns number of sigma points for each variable in the state x.
   *
   * @return The number of sigma points for each variable in the state x.
   */
  int getNumSigmas();

  /**
   * Computes the sigma points for an unscented Kalman filter given the mean (x) and square-root
   * covariance (s) of the filter.
   *
   * @param x An array of the means.
   * @param s Square-root covariance of the filter.
   * @return Two-dimensional array of sigma points. Each column contains all the sigmas for one
   *     dimension in the problem space. Ordered by Xi_0, Xi_{1..n}, Xi_{n+1..2n}.
   */
  Matrix<States, ?> squareRootSigmaPoints(Matrix<States, N1> x, Matrix<States, States> s);

  /**
   * Returns the weight for each sigma point for the mean.
   *
   * @return the weight for each sigma point for the mean.
   */
  Matrix<?, N1> getWm();

  /**
   * Returns an element of the weight for each sigma point for the mean.
   *
   * @param element Element of vector to return.
   * @return the element i's weight for the mean.
   */
  double getWm(int element);

  /**
   * Returns the weight for each sigma point for the covariance.
   *
   * @return the weight for each sigma point for the covariance.
   */
  Matrix<?, N1> getWc();

  /**
   * Returns an element of the weight for each sigma point for the covariance.
   *
   * @param element Element of vector to return.
   * @return The element I's weight for the covariance.
   */
  double getWc(int element);
}
