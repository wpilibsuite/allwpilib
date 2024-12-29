// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.DARE;
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
 * <p>For more on the underlying math, read <a
 * href="https://file.tavsys.net/control/controls-engineering-in-frc.pdf">https://file.tavsys.net/control/controls-engineering-in-frc.pdf</a>
 * chapter 9 "Stochastic control theory".
 *
 * @param <States> Number of states.
 * @param <Inputs> Number of inputs.
 * @param <Outputs> Number of outputs.
 */
public class KalmanFilter<States extends Num, Inputs extends Num, Outputs extends Num>
    implements KalmanTypeFilter<States, Inputs, Outputs> {
  private final Nat<States> m_states;

  private final LinearSystem<States, Inputs, Outputs> m_plant;
  private Matrix<States, N1> m_xHat;
  private Matrix<States, States> m_P;
  private final Matrix<States, States> m_contQ;
  private final Matrix<Outputs, Outputs> m_contR;
  private double m_dtSeconds;

  private final Matrix<States, States> m_initP;

  /**
   * Constructs a Kalman filter with the given plant.
   *
   * <p>See <a
   * href="https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices">https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices</a>
   * for how to select the standard deviations.
   *
   * @param states A Nat representing the states of the system.
   * @param outputs A Nat representing the outputs of the system.
   * @param plant The plant used for the prediction step.
   * @param stateStdDevs Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dtSeconds Nominal discretization timestep.
   * @throws IllegalArgumentException If the system is undetectable.
   */
  public KalmanFilter(
      Nat<States> states,
      Nat<Outputs> outputs,
      LinearSystem<States, Inputs, Outputs> plant,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> measurementStdDevs,
      double dtSeconds) {
    this.m_states = states;

    this.m_plant = plant;

    m_contQ = StateSpaceUtil.makeCovarianceMatrix(states, stateStdDevs);
    m_contR = StateSpaceUtil.makeCovarianceMatrix(outputs, measurementStdDevs);
    m_dtSeconds = dtSeconds;

    // Find discrete A and Q
    var pair = Discretization.discretizeAQ(plant.getA(), m_contQ, dtSeconds);
    var discA = pair.getFirst();
    var discQ = pair.getSecond();

    var discR = Discretization.discretizeR(m_contR, dtSeconds);

    var C = plant.getC();

    m_initP = new Matrix<>(DARE.dare(discA.transpose(), C.transpose(), discQ, discR));

    reset();
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
   * @return the value of the state estimate x-hat at that row.
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
  public final void reset() {
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
    // Find discrete A and Q
    final var discPair = Discretization.discretizeAQ(m_plant.getA(), m_contQ, dtSeconds);
    final var discA = discPair.getFirst();
    final var discQ = discPair.getSecond();

    m_xHat = m_plant.calculateX(m_xHat, u, dtSeconds);

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
    correct(u, y, m_contR);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurement noise covariances vary.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param R Continuous measurement noise covariance matrix.
   */
  public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y, Matrix<Outputs, Outputs> R) {
    final var C = m_plant.getC();
    final var D = m_plant.getD();

    final var discR = Discretization.discretizeR(R, m_dtSeconds);

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
    final Matrix<States, Outputs> K = S.transpose().solve(C.times(m_P.transpose())).transpose();

    // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + K(y − (Cx̂ₖ₊₁⁻ + Duₖ₊₁))
    m_xHat = m_xHat.plus(K.times(y.minus(C.times(m_xHat).plus(D.times(u)))));

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
