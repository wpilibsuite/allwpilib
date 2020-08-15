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
import edu.wpi.first.wpiutil.math.Vector;
import edu.wpi.first.wpiutil.math.numbers.N1;


/**
 * Contains the controller coefficients and logic for a linear-quadratic
 * regulator (LQR).
 * LQRs use the control law u = K(r - x).
 *
 * <p>For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 */
@SuppressWarnings("ClassTypeParameterName")
public class LinearQuadraticRegulator<States extends Num, Inputs extends Num,
      Outputs extends Num> {
  /**
   * The current reference state.
   */
  @SuppressWarnings("MemberName")
  private Matrix<States, N1> m_r;

  /**
   * The computed and capped controller output.
   */
  @SuppressWarnings("MemberName")
  private Matrix<Inputs, N1> m_u;

  // Controller gain.
  @SuppressWarnings("MemberName")
  private Matrix<Inputs, States> m_K;

  /**
   * Constructs a controller with the given coefficients and plant. Rho is defaulted to 1.
   *
   * @param plant     The plant being controlled.
   * @param qelms     The maximum desired error tolerance for each state.
   * @param relms     The maximum desired control effort for each input.
   * @param dtSeconds Discretization timestep.
   */
  public LinearQuadraticRegulator(
        LinearSystem<States, Inputs, Outputs> plant,
        Vector<States> qelms,
        Vector<Inputs> relms,
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
        LinearSystem<States, Inputs, Outputs> plant,
        Vector<States> qelms,
        double rho,
        Vector<Inputs> relms,
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
  public LinearQuadraticRegulator(Matrix<States, States> A, Matrix<States, Inputs> B,
                                  Vector<States> qelms, Vector<Inputs> relms,
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
  public LinearQuadraticRegulator(Matrix<States, States> A, Matrix<States, Inputs> B,
                                  Vector<States> qelms, double rho, Vector<Inputs> relms,
                                  double dtSeconds
  ) {
    this(A, B, StateSpaceUtil.makeCostMatrix(qelms).times(rho),
        StateSpaceUtil.makeCostMatrix(relms), dtSeconds);
  }

  /**
   * Constructs a controller with the given coefficients and plant.
   * @param A         Continuous system matrix of the plant being controlled.
   * @param B         Continuous input matrix of the plant being controlled.
   * @param Q         The state cost matrix.
   * @param R         The input cost matrix.
   * @param dtSeconds Discretization timestep.
   */
  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  public LinearQuadraticRegulator(Matrix<States, States> A, Matrix<States, Inputs> B,
                                  Matrix<States, States> Q, Matrix<Inputs, Inputs> R,
                                  double dtSeconds
  ) {
    var discABPair = Discretization.discretizeAB(A, B, dtSeconds);
    var discA = discABPair.getFirst();
    var discB = discABPair.getSecond();

    var S = Drake.discreteAlgebraicRiccatiEquation(discA, discB, Q, R);

    var temp = discB.transpose().times(S).times(discB).plus(R);

    m_K = temp.solve(discB.transpose().times(S).times(discA));

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
      Nat<States> states, Nat<Inputs> inputs,
      Matrix<Inputs, States> k
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
  public Matrix<Inputs, N1> getU() {
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
  public Matrix<States, N1> getR() {
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
  public Matrix<Inputs, States> getK() {
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
  public Matrix<Inputs, N1> calculate(Matrix<States, N1> x) {
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
  public Matrix<Inputs, N1> calculate(Matrix<States, N1> x, Matrix<States, N1> nextR) {
    m_r = nextR;
    return calculate(x);
  }
}
