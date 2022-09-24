// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Drake;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.Discretization;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.NumericalJacobian;
import java.util.function.BiFunction;

/**
 * A Kalman filter combines predictions from a model and measurements to give an estimate of the
 * true system state. This is useful because many states cannot be measured directly as a result of
 * sensor noise, or because the state is "hidden".
 *
 * <p>Kalman filters use a K gain matrix to determine whether to trust the model or measurements
 * more. Kalman filter theory uses statistics to compute an optimal K gain which minimizes the sum
 * of squares error in the state estimate. This K gain is used to correct the state estimate by some
 * amount of the difference between the actual measurements and the measurements predicted by the
 * model.
 *
 * <p>An extended Kalman filter supports nonlinear state and measurement models. It propagates the
 * error covariance by linearizing the models around the state estimate, then applying the linear
 * Kalman filter equations.
 *
 * <p>For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf chapter 9 "Stochastic control
 * theory".
 */
public class ExtendedKalmanFilter<States extends Num, Inputs extends Num, Outputs extends Num>
    implements KalmanTypeFilter<States, Inputs, Outputs> {
  private final Nat<States> m_states;
  private final Nat<Outputs> m_outputs;

  private final BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> m_f;

  private final BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> m_h;

  private BiFunction<Matrix<Outputs, N1>, Matrix<Outputs, N1>, Matrix<Outputs, N1>> m_residualFuncY;
  private BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> m_addFuncX;

  private final Matrix<States, States> m_contQ;
  private final Matrix<States, States> m_initP;
  private final Matrix<Outputs, Outputs> m_contR;

  private Matrix<States, N1> m_xHat;

  private Matrix<States, States> m_P;

  private double m_dtSeconds;

  /**
   * Constructs an extended Kalman filter.
   *
   * @param states a Nat representing the number of states.
   * @param inputs a Nat representing the number of inputs.
   * @param outputs a Nat representing the number of outputs.
   * @param f A vector-valued function of x and u that returns the derivative of the state vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param stateStdDevs Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dtSeconds Nominal discretization timestep.
   */
  public ExtendedKalmanFilter(
      Nat<States> states,
      Nat<Inputs> inputs,
      Nat<Outputs> outputs,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> h,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> measurementStdDevs,
      double dtSeconds) {
    this(
        states,
        inputs,
        outputs,
        f,
        h,
        stateStdDevs,
        measurementStdDevs,
        Matrix::minus,
        Matrix::plus,
        dtSeconds);
  }

  /**
   * Constructs an extended Kalman filter.
   *
   * @param states a Nat representing the number of states.
   * @param inputs a Nat representing the number of inputs.
   * @param outputs a Nat representing the number of outputs.
   * @param f A vector-valued function of x and u that returns the derivative of the state vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param stateStdDevs Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param residualFuncY A function that computes the residual of two measurement vectors (i.e. it
   *     subtracts them.)
   * @param addFuncX A function that adds two state vectors.
   * @param dtSeconds Nominal discretization timestep.
   */
  public ExtendedKalmanFilter(
      Nat<States> states,
      Nat<Inputs> inputs,
      Nat<Outputs> outputs,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> h,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> measurementStdDevs,
      BiFunction<Matrix<Outputs, N1>, Matrix<Outputs, N1>, Matrix<Outputs, N1>> residualFuncY,
      BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> addFuncX,
      double dtSeconds) {
    m_states = states;
    m_outputs = outputs;

    m_f = f;
    m_h = h;

    m_residualFuncY = residualFuncY;
    m_addFuncX = addFuncX;

    m_contQ = StateSpaceUtil.makeCovarianceMatrix(states, stateStdDevs);
    m_contR = StateSpaceUtil.makeCovarianceMatrix(outputs, measurementStdDevs);
    m_dtSeconds = dtSeconds;

    reset();

    final var contA =
        NumericalJacobian.numericalJacobianX(
            states, states, f, m_xHat, new Matrix<>(inputs, Nat.N1()));
    final var C =
        NumericalJacobian.numericalJacobianX(
            outputs, states, h, m_xHat, new Matrix<>(inputs, Nat.N1()));

    final var discPair = Discretization.discretizeAQTaylor(contA, m_contQ, dtSeconds);
    final var discA = discPair.getFirst();
    final var discQ = discPair.getSecond();

    final var discR = Discretization.discretizeR(m_contR, dtSeconds);

    if (StateSpaceUtil.isDetectable(discA, C) && outputs.getNum() <= states.getNum()) {
      m_initP =
          Drake.discreteAlgebraicRiccatiEquation(discA.transpose(), C.transpose(), discQ, discR);
    } else {
      m_initP = new Matrix<>(states, states);
    }

    m_P = m_initP;
  }

  /**
   * Returns the error covariance matrix P.
   *
   * @return the error covariance matrix P.
   */
  @Override
  public Matrix<States, States> getP() {
    return m_P;
  }

  /**
   * Returns an element of the error covariance matrix P.
   *
   * @param row Row of P.
   * @param col Column of P.
   * @return the value of the error covariance matrix P at (i, j).
   */
  @Override
  public double getP(int row, int col) {
    return m_P.get(row, col);
  }

  /**
   * Sets the entire error covariance matrix P.
   *
   * @param newP The new value of P to use.
   */
  @Override
  public void setP(Matrix<States, States> newP) {
    m_P = newP;
  }

  /**
   * Returns the state estimate x-hat.
   *
   * @return the state estimate x-hat.
   */
  @Override
  public Matrix<States, N1> getXhat() {
    return m_xHat;
  }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param row Row of x-hat.
   * @return the value of the state estimate x-hat at i.
   */
  @Override
  public double getXhat(int row) {
    return m_xHat.get(row, 0);
  }

  /**
   * Set initial state estimate x-hat.
   *
   * @param xHat The state estimate x-hat.
   */
  @Override
  public void setXhat(Matrix<States, N1> xHat) {
    m_xHat = xHat;
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param row Row of x-hat.
   * @param value Value for element of x-hat.
   */
  @Override
  public void setXhat(int row, double value) {
    m_xHat.set(row, 0, value);
  }

  @Override
  public void reset() {
    m_xHat = new Matrix<>(m_states, Nat.N1());
    m_P = m_initP;
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u New control input from controller.
   * @param dtSeconds Timestep for prediction.
   */
  @Override
  public void predict(Matrix<Inputs, N1> u, double dtSeconds) {
    predict(u, m_f, dtSeconds);
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u New control input from controller.
   * @param f The function used to linearlize the model.
   * @param dtSeconds Timestep for prediction.
   */
  public void predict(
      Matrix<Inputs, N1> u,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      double dtSeconds) {
    // Find continuous A
    final var contA = NumericalJacobian.numericalJacobianX(m_states, m_states, f, m_xHat, u);

    // Find discrete A and Q
    final var discPair = Discretization.discretizeAQTaylor(contA, m_contQ, dtSeconds);
    final var discA = discPair.getFirst();
    final var discQ = discPair.getSecond();

    m_xHat = NumericalIntegration.rk4(f, m_xHat, u, dtSeconds);

    // Pₖ₊₁⁻ = APₖ⁻Aᵀ + Q
    m_P = discA.times(m_P).times(discA.transpose()).plus(discQ);

    m_dtSeconds = dtSeconds;
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  @Override
  public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y) {
    correct(m_outputs, u, y, m_h, m_contR, m_residualFuncY, m_addFuncX);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurements available during a timestep's Correct() call vary.
   * The h(x, u) passed to the constructor is used if one is not provided (the two-argument version
   * of this function).
   *
   * @param <Rows> Number of rows in the result of f(x, u).
   * @param rows Number of rows in the result of f(x, u).
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param contR Continuous measurement noise covariance matrix.
   */
  public <Rows extends Num> void correct(
      Nat<Rows> rows,
      Matrix<Inputs, N1> u,
      Matrix<Rows, N1> y,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Rows, N1>> h,
      Matrix<Rows, Rows> contR) {
    correct(rows, u, y, h, contR, Matrix::minus, Matrix::plus);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurements available during a timestep's Correct() call vary.
   * The h(x, u) passed to the constructor is used if one is not provided (the two-argument version
   * of this function).
   *
   * @param <Rows> Number of rows in the result of f(x, u).
   * @param rows Number of rows in the result of f(x, u).
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param contR Continuous measurement noise covariance matrix.
   * @param residualFuncY A function that computes the residual of two measurement vectors (i.e. it
   *     subtracts them.)
   * @param addFuncX A function that adds two state vectors.
   */
  public <Rows extends Num> void correct(
      Nat<Rows> rows,
      Matrix<Inputs, N1> u,
      Matrix<Rows, N1> y,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Rows, N1>> h,
      Matrix<Rows, Rows> contR,
      BiFunction<Matrix<Rows, N1>, Matrix<Rows, N1>, Matrix<Rows, N1>> residualFuncY,
      BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> addFuncX) {
    final var C = NumericalJacobian.numericalJacobianX(rows, m_states, h, m_xHat, u);
    final var discR = Discretization.discretizeR(contR, m_dtSeconds);

    final var S = C.times(m_P).times(C.transpose()).plus(discR);

    // We want to put K = PCᵀS⁻¹ into Ax = b form so we can solve it more
    // efficiently.
    //
    // K = PCᵀS⁻¹
    // KS = PCᵀ
    // (KS)ᵀ = (PCᵀ)ᵀ
    // SᵀKᵀ = CPᵀ
    //
    // The solution of Ax = b can be found via x = A.solve(b).
    //
    // Kᵀ = Sᵀ.solve(CPᵀ)
    // K = (Sᵀ.solve(CPᵀ))ᵀ
    final Matrix<States, Rows> K = S.transpose().solve(C.times(m_P.transpose())).transpose();

    // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + K(y − h(x̂ₖ₊₁⁻, uₖ₊₁))
    m_xHat = addFuncX.apply(m_xHat, K.times(residualFuncY.apply(y, h.apply(m_xHat, u))));

    // Pₖ₊₁⁺ = (I−Kₖ₊₁C)Pₖ₊₁⁻(I−Kₖ₊₁C)ᵀ + Kₖ₊₁RKₖ₊₁ᵀ
    // Use Joseph form for numerical stability
    m_P =
        Matrix.eye(m_states)
            .minus(K.times(C))
            .times(m_P)
            .times(Matrix.eye(m_states).minus(K.times(C)).transpose())
            .plus(K.times(discR).times(K.transpose()));
  }
}
