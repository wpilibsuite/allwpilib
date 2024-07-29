// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.NumericalJacobian;
import java.util.function.BiFunction;
import java.util.function.Function;

/**
 * Constructs a control-affine plant inversion model-based feedforward from given model dynamics.
 *
 * <p>If given the vector valued function as f(x, u) where x is the state vector and u is the input
 * vector, the B matrix(continuous input matrix) is calculated through a {@link NumericalJacobian}.
 * In this case f has to be control-affine (of the form f(x) + Bu).
 *
 * <p>The feedforward is calculated as <strong> u_ff = B<sup>+</sup> (rDot - f(x))</strong>, where
 * <strong> B<sup>+</sup> </strong> is the pseudoinverse of B.
 *
 * <p>This feedforward does not account for a dynamic B matrix, B is either determined or supplied
 * when the feedforward is created and remains constant.
 *
 * <p>For more on the underlying math, read <a
 * href="https://file.tavsys.net/control/controls-engineering-in-frc.pdf">https://file.tavsys.net/control/controls-engineering-in-frc.pdf</a>.
 *
 * @param <States> Number of states.
 * @param <Inputs> Number of inputs.
 */
public class ControlAffinePlantInversionFeedforward<States extends Num, Inputs extends Num> {
  /** The current reference state. */
  private Matrix<States, N1> m_r;

  /** The computed feedforward. */
  private Matrix<Inputs, N1> m_uff;

  private final Matrix<States, Inputs> m_B;

  private final Nat<Inputs> m_inputs;

  private final double m_dt;

  /** The model dynamics. */
  private final BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> m_f;

  /**
   * Constructs a feedforward with given model dynamics as a function of state and input.
   *
   * @param states A {@link Nat} representing the number of states.
   * @param inputs A {@link Nat} representing the number of inputs.
   * @param f A vector-valued function of x, the state, and u, the input, that returns the
   *     derivative of the state vector. HAS to be control-affine (of the form f(x) + Bu).
   * @param dtSeconds The timestep between calls of calculate().
   */
  public ControlAffinePlantInversionFeedforward(
      Nat<States> states,
      Nat<Inputs> inputs,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      double dtSeconds) {
    this.m_dt = dtSeconds;
    this.m_f = f;
    this.m_inputs = inputs;

    this.m_B =
        NumericalJacobian.numericalJacobianU(
            states, inputs, m_f, new Matrix<>(states, Nat.N1()), new Matrix<>(inputs, Nat.N1()));

    m_r = new Matrix<>(states, Nat.N1());
    m_uff = new Matrix<>(inputs, Nat.N1());

    reset();
  }

  /**
   * Constructs a feedforward with given model dynamics as a function of state, and the plant's
   * B(continuous input matrix) matrix.
   *
   * @param states A {@link Nat} representing the number of states.
   * @param inputs A {@link Nat} representing the number of inputs.
   * @param f A vector-valued function of x, the state, that returns the derivative of the state
   *     vector.
   * @param B Continuous input matrix of the plant being controlled.
   * @param dtSeconds The timestep between calls of calculate().
   */
  public ControlAffinePlantInversionFeedforward(
      Nat<States> states,
      Nat<Inputs> inputs,
      Function<Matrix<States, N1>, Matrix<States, N1>> f,
      Matrix<States, Inputs> B,
      double dtSeconds) {
    this.m_dt = dtSeconds;
    this.m_inputs = inputs;

    this.m_f = (x, u) -> f.apply(x);
    this.m_B = B;

    m_r = new Matrix<>(states, Nat.N1());
    m_uff = new Matrix<>(inputs, Nat.N1());

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
    var rDot = nextR.minus(r).div(m_dt);

    // ṙ = f(r) + Bu
    // Bu = ṙ − f(r)
    // u = B⁺(ṙ − f(r))
    m_uff = m_B.solve(rDot.minus(m_f.apply(r, new Matrix<>(m_inputs, Nat.N1()))));

    m_r = nextR;
    return m_uff;
  }
}
