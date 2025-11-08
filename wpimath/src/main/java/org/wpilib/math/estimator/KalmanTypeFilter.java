// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.numbers.N1;

/**
 * Interface for Kalman filters for use with KalmanFilterLatencyCompensator.
 *
 * @param <States> The number of states.
 * @param <Inputs> The number of inputs.
 * @param <Outputs> The number of outputs.
 */
public interface KalmanTypeFilter<States extends Num, Inputs extends Num, Outputs extends Num> {
  /**
   * Returns the error covariance matrix.
   *
   * @return The error covariance matrix.
   */
  Matrix<States, States> getP();

  /**
   * Returns an element of the error covariance matrix.
   *
   * @param i The row.
   * @param j The column.
   * @return An element of the error covariance matrix.
   */
  double getP(int i, int j);

  /**
   * Sets the error covariance matrix.
   *
   * @param newP The error covariance matrix.
   */
  void setP(Matrix<States, States> newP);

  /**
   * Returns the state estimate.
   *
   * @return The state estimate.
   */
  Matrix<States, N1> getXhat();

  /**
   * Returns an element of the state estimate.
   *
   * @param i The row.
   * @return An element of the state estimate.
   */
  double getXhat(int i);

  /**
   * Sets the state estimate.
   *
   * @param xHat The state estimate.
   */
  void setXhat(Matrix<States, N1> xHat);

  /**
   * Sets an element of the state estimate.
   *
   * @param i The row.
   * @param value The value.
   */
  void setXhat(int i, double value);

  /** Resets the observer. */
  void reset();

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u New control input from controller.
   * @param dt Timestep for prediction in seconds.
   */
  void predict(Matrix<Inputs, N1> u, double dt);

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y);
}
