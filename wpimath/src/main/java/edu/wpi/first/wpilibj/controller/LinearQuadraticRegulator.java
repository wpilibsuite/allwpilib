/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.math.Drake;
import edu.wpi.first.wpilibj.math.Discretization;
import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

/**
 * Contains the controller coefficients and logic for a linear-quadratic
 * regulator (LQR).
 * LQRs use the control law u = K(r - x).
 *
 * <p>For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 */
public class LinearQuadraticRegulator<S extends Num, I extends Num,
      O extends Num> {
  /**
   * The current reference state.
   */
  @SuppressWarnings("MemberName")
  private Matrix<S, N1> m_r;

  /**
   * The computed and capped controller output.
   */
  @SuppressWarnings("MemberName")
  private Matrix<I, N1> m_u;

  // Controller gain.
  @SuppressWarnings("MemberName")
  private Matrix<I, S> m_K;

  /**
   * Constructs a controller with the given coefficients and plant. Rho is defaulted to 1.
   *
   * @param plant     The plant being controlled.
   * @param qelms     The maximum desired error tolerance for each state.
   * @param relms     The maximum desired control effort for each input.
   * @param dtSeconds Discretization timestep.
   */
  public LinearQuadraticRegulator(
        LinearSystem<S, I, O> plant,
        Matrix<S, N1> qelms,
        Matrix<I, N1> relms,
        double dtSeconds
  ) {
    this(plant.getA(), plant.getB(), qelms, 1.0, relms, dtSeconds);
  }

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param plant     The plant being controlled.
   * @param qelms     The maximum desired error tolerance for each state.
   * @param rho       A weighting factor that balances control effort and state excursion.
   *                  Greater values penalize state excursion more heavily. 1 is a good starting
   *                  value.
   * @param relms     The maximum desired control effort for each input.
   * @param dtSeconds Discretization timestep.
   */
  public LinearQuadraticRegulator(
        LinearSystem<S, I, O> plant,
        Matrix<S, N1> qelms,
        double rho,
        Matrix<I, N1> relms,
        double dtSeconds
  ) {
    this(plant.getA(), plant.getB(), qelms, rho, relms, dtSeconds);
  }

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A         Continuous system matrix of the plant being controlled.
   * @param B         Continuous input matrix of the plant being controlled.
   * @param qelms     The maximum desired error tolerance for each state.
   * @param relms     The maximum desired control effort for each input.
   * @param dtSeconds Discretization timestep.
   */
  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  public LinearQuadraticRegulator(Matrix<S, S> A, Matrix<S, I> B,
                                  Matrix<S, N1> qelms, Matrix<I, N1> relms,
                                  double dtSeconds
  ) {
    this(A, B, qelms, 1.0, relms, dtSeconds);
  }

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A         Continuous system matrix of the plant being controlled.
   * @param B         Continuous input matrix of the plant being controlled.
   * @param qelms     The maximum desired error tolerance for each state.
   * @param rho       A weighting factor that balances control effort and state excursion.
   *                  Greater
   *                  values penalize state excursion more heavily. 1 is a good starting value.
   * @param relms     The maximum desired control effort for each input.
   * @param dtSeconds Discretization timestep.
   */
  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  public LinearQuadraticRegulator(Matrix<S, S> A, Matrix<S, I> B,
                                  Matrix<S, N1> qelms, double rho, Matrix<I, N1> relms,
                                  double dtSeconds
  ) {
    var Q = StateSpaceUtil.makeCostMatrix(qelms).times(rho);
    var R = StateSpaceUtil.makeCostMatrix(relms);

    var discABPair = Discretization.discretizeAB(A, B, dtSeconds);
    var discA = discABPair.getFirst();
    var discB = discABPair.getSecond();

    var S = Drake.discreteAlgebraicRiccatiEquation(discA, discB, Q, R);

    var temp = discB.transpose().times(new Matrix<S, S>(S)).times(discB).plus(R);

    m_K = temp.solve(discB.transpose().times(new Matrix<S, S>(S)).times(discA));

    m_r = new Matrix<>(new SimpleMatrix(B.getNumRows(), 1));
    m_u = new Matrix<>(new SimpleMatrix(B.getNumCols(), 1));

    reset();
  }

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param states The number of states.
   * @param inputs The number of inputs.
   * @param k The gain matrix.
   */
  @SuppressWarnings("ParameterName")
  public LinearQuadraticRegulator(
        Nat<S> states, Nat<I> inputs,
        Matrix<I, S> k
  ) {
    m_K = k;

    m_r = new Matrix<>(states, Nat.N1());
    m_u = new Matrix<>(inputs, Nat.N1());

    reset();
  }

  /**
   * Returns the control input vector u.
   *
   * @return The control input.
   */
  public Matrix<I, N1> getU() {
    return m_u;
  }

  /**
   * Returns an element of the control input vector u.
   *
   * @param row Row of u.
   *
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
  public Matrix<S, N1> getR() {
    return m_r;
  }

  /**
   * Returns an element of the reference vector r.
   *
   * @param row Row of r.
   *
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
  public Matrix<I, S> getK() {
    return m_K;
  }

  /**
   * Resets the controller.
   */
  public void reset() {
    m_r.fill(0.0);
    m_u.fill(0.0);
  }

  /**
   * Returns the next output of the controller.
   *
   * @param x The current state x.
   */
  @SuppressWarnings("ParameterName")
  public Matrix<I, N1> calculate(Matrix<S, N1> x) {
    m_u = m_K.times(m_r.minus(x));
    return m_u;
  }

  /**
   * Returns the next output of the controller.
   *
   * @param x     The current state x.
   * @param nextR the next reference vector r.
   */
  @SuppressWarnings("ParameterName")
  public Matrix<I, N1> calculate(Matrix<S, N1> x, Matrix<S, N1> nextR) {
    m_r = nextR;
    return calculate(x);
  }
}
