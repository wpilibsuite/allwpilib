// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.linalg.DARE;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.Vector;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.system.Discretization;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.math.util.Num;
import org.wpilib.math.util.StateSpaceUtil;

/**
 * Contains the controller coefficients and logic for a linear-quadratic regulator (LQR). LQRs use
 * the control law u = K(r - x).
 *
 * <p>For more on the underlying math, read <a
 * href="https://file.tavsys.net/control/controls-engineering-in-frc.pdf">https://file.tavsys.net/control/controls-engineering-in-frc.pdf</a>.
 *
 * @param <States> Number of states.
 * @param <Inputs> Number of inputs.
 * @param <Outputs> Number of outputs.
 */
public class LinearQuadraticRegulator<States extends Num, Inputs extends Num, Outputs extends Num> {
  /** The current reference state. */
  private Matrix<States, N1> m_r;

  /** The computed and capped controller output. */
  private Matrix<Inputs, N1> m_u;

  // Controller gain.
  private Matrix<Inputs, States> m_K;

  /**
   * Constructs a controller with the given coefficients and plant. Rho is defaulted to 1.
   *
   * <p>See <a
   * href="https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-intro.html#lqr-tuning">https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-intro.html#lqr-tuning</a>
   * for how to select the tolerances.
   *
   * @param plant The plant being controlled.
   * @param qelms The maximum desired error tolerance for each state.
   * @param relms The maximum desired control effort for each input.
   * @param dt Discretization timestep in seconds.
   * @throws IllegalArgumentException If the system is unstabilizable.
   */
  public LinearQuadraticRegulator(
      LinearSystem<States, Inputs, Outputs> plant,
      Vector<States> qelms,
      Vector<Inputs> relms,
      double dt) {
    this(
        plant.getA(),
        plant.getB(),
        StateSpaceUtil.makeCostMatrix(qelms),
        StateSpaceUtil.makeCostMatrix(relms),
        dt);
  }

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * <p>See <a
   * href="https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-intro.html#lqr-tuning">https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-intro.html#lqr-tuning</a>
   * for how to select the tolerances.
   *
   * @param A Continuous system matrix of the plant being controlled.
   * @param B Continuous input matrix of the plant being controlled.
   * @param qelms The maximum desired error tolerance for each state.
   * @param relms The maximum desired control effort for each input.
   * @param dt Discretization timestep in seconds.
   * @throws IllegalArgumentException If the system is unstabilizable.
   */
  public LinearQuadraticRegulator(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Vector<States> qelms,
      Vector<Inputs> relms,
      double dt) {
    this(A, B, StateSpaceUtil.makeCostMatrix(qelms), StateSpaceUtil.makeCostMatrix(relms), dt);
  }

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A Continuous system matrix of the plant being controlled.
   * @param B Continuous input matrix of the plant being controlled.
   * @param Q The state cost matrix.
   * @param R The input cost matrix.
   * @param dt Discretization timestep in seconds.
   * @throws IllegalArgumentException If the system is unstabilizable.
   */
  public LinearQuadraticRegulator(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      double dt) {
    var discABPair = Discretization.discretizeAB(A, B, dt);
    var discA = discABPair.getFirst();
    var discB = discABPair.getSecond();

    var S = DARE.dare(discA, discB, Q, R);

    // K = (BᵀSB + R)⁻¹BᵀSA
    m_K =
        discB
            .transpose()
            .times(S)
            .times(discB)
            .plus(R)
            .solve(discB.transpose().times(S).times(discA));

    m_r = new Matrix<>(new SimpleMatrix(B.getNumRows(), 1));
    m_u = new Matrix<>(new SimpleMatrix(B.getNumCols(), 1));

    reset();
  }

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A Continuous system matrix of the plant being controlled.
   * @param B Continuous input matrix of the plant being controlled.
   * @param Q The state cost matrix.
   * @param R The input cost matrix.
   * @param N The state-input cross-term cost matrix.
   * @param dt Discretization timestep in seconds.
   * @throws IllegalArgumentException If the system is unstabilizable.
   */
  public LinearQuadraticRegulator(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, Inputs> N,
      double dt) {
    var discABPair = Discretization.discretizeAB(A, B, dt);
    var discA = discABPair.getFirst();
    var discB = discABPair.getSecond();

    var S = DARE.dare(discA, discB, Q, R, N);

    // K = (BᵀSB + R)⁻¹(BᵀSA + Nᵀ)
    m_K =
        discB
            .transpose()
            .times(S)
            .times(discB)
            .plus(R)
            .solve(discB.transpose().times(S).times(discA).plus(N.transpose()));

    m_r = new Matrix<>(new SimpleMatrix(B.getNumRows(), 1));
    m_u = new Matrix<>(new SimpleMatrix(B.getNumCols(), 1));

    reset();
  }

  /**
   * Returns the control input vector u.
   *
   * @return The control input.
   */
  public Matrix<Inputs, N1> getU() {
    return m_u;
  }

  /**
   * Returns an element of the control input vector u.
   *
   * @param row Row of u.
   * @return The row of the control input vector.
   */
  public double getU(int row) {
    return m_u.get(row, 0);
  }

  /**
   * Returns the reference vector r.
   *
   * @return The reference vector.
   */
  public Matrix<States, N1> getR() {
    return m_r;
  }

  /**
   * Returns an element of the reference vector r.
   *
   * @param row Row of r.
   * @return The row of the reference vector.
   */
  public double getR(int row) {
    return m_r.get(row, 0);
  }

  /**
   * Returns the controller matrix K.
   *
   * @return the controller matrix K.
   */
  public Matrix<Inputs, States> getK() {
    return m_K;
  }

  /** Resets the controller. */
  public final void reset() {
    m_r.fill(0.0);
    m_u.fill(0.0);
  }

  /**
   * Returns the next output of the controller.
   *
   * @param x The current state x.
   * @return The next controller output.
   */
  public Matrix<Inputs, N1> calculate(Matrix<States, N1> x) {
    m_u = m_K.times(m_r.minus(x));
    return m_u;
  }

  /**
   * Returns the next output of the controller.
   *
   * @param x The current state x.
   * @param nextR the next reference vector r.
   * @return The next controller output.
   */
  public Matrix<Inputs, N1> calculate(Matrix<States, N1> x, Matrix<States, N1> nextR) {
    m_r = nextR;
    return calculate(x);
  }

  /**
   * Adjusts LQR controller gain to compensate for a pure time delay in the input.
   *
   * <p>Linear-Quadratic regulator controller gains tend to be aggressive. If sensor measurements
   * are time-delayed too long, the LQR may be unstable. However, if we know the amount of delay, we
   * can compute the control based on where the system will be after the time delay.
   *
   * <p>See <a
   * href="https://file.tavsys.net/control/controls-engineering-in-frc.pdf">https://file.tavsys.net/control/controls-engineering-in-frc.pdf</a>
   * appendix C.4 for a derivation.
   *
   * @param plant The plant being controlled.
   * @param dt Discretization timestep in seconds.
   * @param inputDelay Input time delay in seconds.
   */
  public void latencyCompensate(
      LinearSystem<States, Inputs, Outputs> plant, double dt, double inputDelay) {
    var discABPair = Discretization.discretizeAB(plant.getA(), plant.getB(), dt);
    var discA = discABPair.getFirst();
    var discB = discABPair.getSecond();

    m_K = m_K.times((discA.minus(discB.times(m_K))).pow(inputDelay / dt));
  }
}
