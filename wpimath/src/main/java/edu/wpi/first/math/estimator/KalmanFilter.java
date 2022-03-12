// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Drake;
import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.Discretization;
import edu.wpi.first.math.system.LinearSystem;

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
 * <p>For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf chapter 9 "Stochastic control
 * theory".
 */
@SuppressWarnings("ClassTypeParameterName")
public class KalmanFilter<States extends Num, Inputs extends Num, Outputs extends Num> {
  private final Nat<States> m_states;

  private final LinearSystem<States, Inputs, Outputs> m_plant;

  /** The steady-state Kalman gain matrix. */
  @SuppressWarnings("MemberName")
  private Matrix<States, Outputs> m_K;

  /** The state estimate. */
  @SuppressWarnings("MemberName")
  private Matrix<States, N1> m_xHat;

  private Matrix<States, States> m_P;

  private Matrix<States, States> m_discA;
  private Matrix<States, States> m_discQ;

  private Matrix<Outputs, Outputs> m_discR;

  /**
   * Constructs a state-space observer with the given plant.
   *
   * @param states A Nat representing the states of the system.
   * @param outputs A Nat representing the outputs of the system.
   * @param plant The plant used for the prediction step.
   * @param stateStdDevs Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dtSeconds Nominal discretization timestep.
   */
  @SuppressWarnings("LocalVariableName")
  public KalmanFilter(
      Nat<States> states,
      Nat<Outputs> outputs,
      LinearSystem<States, Inputs, Outputs> plant,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> measurementStdDevs,
      double dtSeconds) {
    this.m_states = states;

    this.m_plant = plant;

    var contQ = StateSpaceUtil.makeCovarianceMatrix(states, stateStdDevs);
    var contR = StateSpaceUtil.makeCovarianceMatrix(outputs, measurementStdDevs);

    var pair = Discretization.discretizeAQTaylor(plant.getA(), contQ, dtSeconds);
    m_discA = pair.getFirst();
    m_discQ = pair.getSecond();

    m_discR = Discretization.discretizeR(contR, dtSeconds);

    var C = plant.getC();

    if (!StateSpaceUtil.isDetectable(m_discA, C)) {
      var builder =
          new StringBuilder("The system passed to the Kalman filter is unobservable!\n\nA =\n");
      builder
          .append(m_discA.getStorage().toString())
          .append("\nC =\n")
          .append(C.getStorage().toString())
          .append('\n');

      var msg = builder.toString();
      MathSharedStore.reportError(msg, Thread.currentThread().getStackTrace());
      throw new IllegalArgumentException(msg);
    }

    m_P = new Matrix<>(
            Drake.discreteAlgebraicRiccatiEquation(m_discA.transpose(), C.transpose(), m_discQ, m_discR));

    // S = CPCᵀ + R
    var S = C.times(m_P).times(C.transpose()).plus(m_discR);

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
    m_K = S.transpose().solve((C.times(m_P.transpose()))).transpose();

    reset();
  }

  public void reset() {
    m_xHat = new Matrix<>(m_states, Nat.N1());
  }

  /**
   * Returns the steady-state Kalman gain matrix K.
   *
   * @return The steady-state Kalman gain matrix K.
   */
  public Matrix<States, Outputs> getK() {
    return m_K;
  }

  /**
   * Returns an element of the steady-state Kalman gain matrix K.
   *
   * @param row Row of K.
   * @param col Column of K.
   * @return the element (i, j) of the steady-state Kalman gain matrix K.
   */
  public double getK(int row, int col) {
    return m_K.get(row, col);
  }

  /**
   * Set initial state estimate x-hat.
   *
   * @param xhat The state estimate x-hat.
   */
  public void setXhat(Matrix<States, N1> xhat) {
    this.m_xHat = xhat;
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param row Row of x-hat.
   * @param value Value for element of x-hat.
   */
  public void setXhat(int row, double value) {
    m_xHat.set(row, 0, value);
  }

  /**
   * Returns the state estimate x-hat.
   *
   * @return The state estimate x-hat.
   */
  public Matrix<States, N1> getXhat() {
    return m_xHat;
  }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param row Row of x-hat.
   * @return the state estimate x-hat at i.
   */
  public double getXhat(int row) {
    return m_xHat.get(row, 0);
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u New control input from controller.
   * @param dtSeconds Timestep for prediction.
   */
  @SuppressWarnings("ParameterName")
  public void predict(Matrix<Inputs, N1> u, double dtSeconds) {
    this.m_xHat = m_plant.calculateX(m_xHat, u, dtSeconds);

    // Pₖ₊₁⁺ = A(Pₖ₊₁⁻)Aᵀ + Q
    m_P = m_discA.times(m_P.times(m_discA.transpose())).plus(m_discQ);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the last predict step.
   * @param y Measurement vector.
   */
  @SuppressWarnings("ParameterName")
  public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y) {
    final var C = m_plant.getC();
    final var D = m_plant.getD();
    
    // S = CPCᵀ + R
    var S = C.times(m_P).times(C.transpose()).plus(m_discR);
    
    // K = (Sᵀ.solve(CPᵀ))ᵀ
    m_K = S.transpose().solve((C.times(m_P.transpose()))).transpose();

    // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + K(y − (Cx̂ₖ₊₁⁻ + Duₖ₊₁))
    m_xHat = m_xHat.plus(m_K.times(y.minus(C.times(m_xHat).plus(D.times(u)))));

    // Pₖ₊₁⁺ = (I − Kₖ₊₁C)Pₖ₊₁⁻
    m_P = Matrix.eye(m_states).minus(m_K.times(C)).times(m_P);
  }
}
