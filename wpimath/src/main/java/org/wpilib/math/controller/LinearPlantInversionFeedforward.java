// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import org.wpilib.math.Matrix;
import org.wpilib.math.Num;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.system.Discretization;
import org.wpilib.math.system.LinearSystem;
import org.ejml.simple.SimpleMatrix;

/**
 * Constructs a plant inversion model-based feedforward from a {@link LinearSystem}.
 *
 * <p>The feedforward is calculated as <strong> u_ff = B<sup>+</sup> (r_k+1 - A r_k) </strong>,
 * where <strong> B<sup>+</sup> </strong> is the pseudoinverse of B.
 *
 * <p>For more on the underlying math, read <a
 * href="https://file.tavsys.net/control/controls-engineering-in-frc.pdf">https://file.tavsys.net/control/controls-engineering-in-frc.pdf</a>.
 *
 * @param <States> Number of states.
 * @param <Inputs> Number of inputs.
 * @param <Outputs> Number of outputs.
 */
public class LinearPlantInversionFeedforward<
    States extends Num, Inputs extends Num, Outputs extends Num> {
  /** The current reference state. */
  private Matrix<States, N1> m_r;

  /** The computed feedforward. */
  private Matrix<Inputs, N1> m_uff;

  private final Matrix<States, Inputs> m_B;

  private final Matrix<States, States> m_A;

  /**
   * Constructs a feedforward with the given plant.
   *
   * @param plant The plant being controlled.
   * @param dt Discretization timestep in seconds.
   */
  public LinearPlantInversionFeedforward(LinearSystem<States, Inputs, Outputs> plant, double dt) {
    this(plant.getA(), plant.getB(), dt);
  }

  /**
   * Constructs a feedforward with the given coefficients.
   *
   * @param A Continuous system matrix of the plant being controlled.
   * @param B Continuous input matrix of the plant being controlled.
   * @param dt Discretization timestep in seconds.
   */
  public LinearPlantInversionFeedforward(
      Matrix<States, States> A, Matrix<States, Inputs> B, double dt) {
    var discABPair = Discretization.discretizeAB(A, B, dt);
    this.m_A = discABPair.getFirst();
    this.m_B = discABPair.getSecond();

    m_r = new Matrix<>(new SimpleMatrix(B.getNumRows(), 1));
    m_uff = new Matrix<>(new SimpleMatrix(B.getNumCols(), 1));

    reset();
  }

  /**
   * Returns the previously calculated feedforward as an input vector.
   *
   * @return The calculated feedforward.
   */
  public Matrix<Inputs, N1> getUff() {
    return m_uff;
  }

  /**
   * Returns an element of the previously calculated feedforward.
   *
   * @param row Row of uff.
   * @return The row of the calculated feedforward.
   */
  public double getUff(int row) {
    return m_uff.get(row, 0);
  }

  /**
   * Returns the current reference vector r.
   *
   * @return The current reference vector.
   */
  public Matrix<States, N1> getR() {
    return m_r;
  }

  /**
   * Returns an element of the current reference vector r.
   *
   * @param row Row of r.
   * @return The row of the current reference vector.
   */
  public double getR(int row) {
    return m_r.get(row, 0);
  }

  /**
   * Resets the feedforward with a specified initial state vector.
   *
   * @param initialState The initial state vector.
   */
  public final void reset(Matrix<States, N1> initialState) {
    m_r = initialState;
    m_uff.fill(0.0);
  }

  /** Resets the feedforward with a zero initial state vector. */
  public final void reset() {
    m_r.fill(0.0);
    m_uff.fill(0.0);
  }

  /**
   * Calculate the feedforward with only the desired future reference. This uses the internally
   * stored "current" reference.
   *
   * <p>If this method is used the initial state of the system is the one set using {@link
   * LinearPlantInversionFeedforward#reset(Matrix)}. If the initial state is not set it defaults to
   * a zero vector.
   *
   * @param nextR The reference state of the future timestep (k + dt).
   * @return The calculated feedforward.
   */
  public Matrix<Inputs, N1> calculate(Matrix<States, N1> nextR) {
    return calculate(m_r, nextR);
  }

  /**
   * Calculate the feedforward with current and future reference vectors.
   *
   * @param r The reference state of the current timestep (k).
   * @param nextR The reference state of the future timestep (k + dt).
   * @return The calculated feedforward.
   */
  public Matrix<Inputs, N1> calculate(Matrix<States, N1> r, Matrix<States, N1> nextR) {
    // rₖ₊₁ = Arₖ + Buₖ
    // Buₖ = rₖ₊₁ − Arₖ
    // uₖ = B⁺(rₖ₊₁ − Arₖ)
    m_uff = new Matrix<>(m_B.solve(nextR.minus(m_A.times(r))));

    m_r = nextR;
    return m_uff;
  }
}
