// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.util.Nat;
import org.wpilib.math.util.Num;
import org.wpilib.math.numbers.N1;
import java.util.function.BiFunction;

/**
 * An Unscented Kalman Filter using sigma points and weights from Van der Merwe's 2004 dissertation.
 * S3UKF is generally preferred due to its greater performance with nearly identical accuracy.
 *
 * @param <States> Number of states.
 * @param <Inputs> Number of inputs.
 * @param <Outputs> Number of outputs.
 */
public class MerweUKF<States extends Num, Inputs extends Num, Outputs extends Num>
    extends UnscentedKalmanFilter<States, Inputs, Outputs> {
  /**
   * Constructs a Merwe Unscented Kalman Filter.
   *
   * <p>See <a
   * href="https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices">https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices</a>
   * for how to select the standard deviations.
   *
   * @param states A Nat representing the number of states.
   * @param outputs A Nat representing the number of outputs.
   * @param f A vector-valued function of x and u that returns the derivative of the state vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param stateStdDevs Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param nominalDt Nominal discretization timestep in seconds.
   */
  public MerweUKF(
      Nat<States> states,
      Nat<Outputs> outputs,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> h,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> measurementStdDevs,
      double nominalDt) {
    super(
        new MerweScaledSigmaPoints<>(states),
        states,
        outputs,
        f,
        h,
        stateStdDevs,
        measurementStdDevs,
        (sigmas, Wm) -> sigmas.times(Matrix.changeBoundsUnchecked(Wm)),
        (sigmas, Wm) -> sigmas.times(Matrix.changeBoundsUnchecked(Wm)),
        Matrix::minus,
        Matrix::minus,
        Matrix::plus,
        nominalDt);
  }

  /**
   * Constructs a Merwe Unscented Kalman filter with custom mean, residual, and addition functions.
   * Using custom functions for arithmetic can be useful if you have angles in the state or
   * measurements, because they allow you to correctly account for the modular nature of angle
   * arithmetic.
   *
   * <p>See <a
   * href="https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices">https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices</a>
   * for how to select the standard deviations.
   *
   * @param states A Nat representing the number of states.
   * @param outputs A Nat representing the number of outputs.
   * @param f A vector-valued function of x and u that returns the derivative of the state vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param stateStdDevs Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param meanFuncX A function that computes the mean of NumSigmas state vectors using a given set
   *     of weights.
   * @param meanFuncY A function that computes the mean of NumSigmas measurement vectors using a
   *     given set of weights.
   * @param residualFuncX A function that computes the residual of two state vectors (i.e. it
   *     subtracts them.)
   * @param residualFuncY A function that computes the residual of two measurement vectors (i.e. it
   *     subtracts them.)
   * @param addFuncX A function that adds two state vectors.
   * @param nominalDt Nominal discretization timestep in seconds.
   */
  public MerweUKF(
      Nat<States> states,
      Nat<Outputs> outputs,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> h,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> measurementStdDevs,
      BiFunction<Matrix<States, ?>, Matrix<?, N1>, Matrix<States, N1>> meanFuncX,
      BiFunction<Matrix<Outputs, ?>, Matrix<?, N1>, Matrix<Outputs, N1>> meanFuncY,
      BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> residualFuncX,
      BiFunction<Matrix<Outputs, N1>, Matrix<Outputs, N1>, Matrix<Outputs, N1>> residualFuncY,
      BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> addFuncX,
      double nominalDt) {
    super(
        new MerweScaledSigmaPoints<>(states),
        states,
        outputs,
        f,
        h,
        stateStdDevs,
        measurementStdDevs,
        meanFuncX,
        meanFuncY,
        residualFuncX,
        residualFuncY,
        addFuncX,
        nominalDt);
  }
}
