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
 * <p>This class assumes predict() and correct() are called in pairs, so the Kalman gain converges
 * to a steady-state value. If they aren't, use {@link KalmanFilter} instead.
 *
 * <p>For more on the underlying math, read <a
 * href="https://file.tavsys.net/control/controls-engineering-in-frc.pdf">https://file.tavsys.net/control/controls-engineering-in-frc.pdf</a>
 * chapter 9 "Stochastic control theory".
 *
 * @param <States> Number of states.
 * @param <Inputs> Number of inputs.
 * @param <Outputs> Number of outputs.
 */
public class SteadyStateKalmanFilter<States extends Num, Inputs extends Num, Outputs extends Num> {
  private final Nat<States> m_states;

  private final LinearSystem<States, Inputs, Outputs> m_plant;

  /** The steady-state Kalman gain matrix. */
  private final Matrix<States, Outputs> m_K;

  /** The state estimate. */
  private Matrix<States, N1> m_xHat;

  /**
   * Constructs a steady-state Kalman filter with the given plant.
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
  public SteadyStateKalmanFilter(
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

    var pair = Discretization.discretizeAQ(plant.getA(), contQ, dtSeconds);
    var discA = pair.getFirst();
    var discQ = pair.getSecond();

    var discR = Discretization.discretizeR(contR, dtSeconds);

    var C = plant.getC();

    var P = new Matrix<>(DARE.dare(discA.transpose(), C.transpose(), discQ, discR));

    // S = CPCᵀ + R
    var S = C.times(P).times(C.transpose()).plus(discR);

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
    m_K =
        new Matrix<>(
            S.transpose().getStorage().solve(C.times(P.transpose()).getStorage()).transpose());

    reset();
  }

  /** Resets the observer. */
  public final void reset() {
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
   * @return the state estimate x-hat at that row.
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
  public void predict(Matrix<Inputs, N1> u, double dtSeconds) {
    this.m_xHat = m_plant.calculateX(m_xHat, u, dtSeconds);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the last predict step.
   * @param y Measurement vector.
   */
  public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y) {
    final var C = m_plant.getC();
    final var D = m_plant.getD();

    // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + K(y − (Cx̂ₖ₊₁⁻ + Duₖ₊₁))
    m_xHat = m_xHat.plus(m_K.times(y.minus(C.times(m_xHat).plus(D.times(u)))));
  }
}
