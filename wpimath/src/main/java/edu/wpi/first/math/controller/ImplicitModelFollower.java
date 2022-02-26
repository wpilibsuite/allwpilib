// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.Discretization;
import edu.wpi.first.math.system.LinearSystem;
import org.ejml.simple.SimpleMatrix;

/**
 * Contains the controller coefficients and logic for an implicit model follower.
 *
 * <p>Implicit model following lets us design a feedback controller that erases the dynamics of our
 * system and makes it behave like some other system. This can be used to make a drivetrain more
 * controllable during teleop driving by making it behave like a slower or more benign drivetrain.
 *
 * <p>For more on the underlying math, read appendix B.3 in
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 */
@SuppressWarnings("ClassTypeParameterName")
public class ImplicitModelFollower<States extends Num, Inputs extends Num, Outputs extends Num> {
  // Computed controller output
  @SuppressWarnings("MemberName")
  private Matrix<Inputs, N1> m_u;

  // State space conversion gain
  @SuppressWarnings("MemberName")
  private Matrix<Inputs, States> m_A;

  // Input space conversion gain
  @SuppressWarnings("MemberName")
  private Matrix<Inputs, Inputs> m_B;

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param plant The plant being controlled.
   * @param plantRef The plant whose dynamics should be followed.
   * @param dtSeconds Discretization timestep.
   */
  public ImplicitModelFollower(
      LinearSystem<States, Inputs, Outputs> plant,
      LinearSystem<States, Inputs, Outputs> plantRef,
      double dtSeconds) {
    this(plant.getA(), plant.getB(), plantRef.getA(), plantRef.getB(), dtSeconds);
  }

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A Continuous system matrix of the plant being controlled.
   * @param B Continuous input matrix of the plant being controlled.
   * @param Aref Continuous system matrix whose dynamics should be followed.
   * @param Bref Continuous input matrix whose dynamics should be followed.
   * @param dtSeconds Discretization timestep.
   */
  @SuppressWarnings("ParameterName")
  public ImplicitModelFollower(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Aref,
      Matrix<States, Inputs> Bref,
      double dtSeconds) {
    m_u = new Matrix<>(new SimpleMatrix(B.getNumCols(), 1));

    // Discretize real dynamics
    var discABPair = Discretization.discretizeAB(A, B, dtSeconds);
    var discA = discABPair.getFirst();
    var discB = discABPair.getSecond();

    // Discretize desired dynamics
    var discABrefPair = Discretization.discretizeAB(Aref, Bref, dtSeconds);
    var discAref = discABrefPair.getFirst();
    var discBref = discABrefPair.getSecond();

    // Find u_imf that makes real model match reference model.
    //
    // x_k+1 = Ax_k + Bu_imf
    // z_k+1 = Aref z_k + Bref u_k
    //
    // Let x_k = z_k.
    //
    // x_k+1 = z_k+1
    // Ax_k + Bu_imf = Aref x_k + Bref u_k
    // Bu_imf = Aref x_k - Ax_k + Bref u_k
    // Bu_imf = (Aref - A)x_k + Bref u_k
    // u_imf = B^+ ((Aref - A)x_k + Bref u_k)
    // u_imf = -B^+ (A - Aref)x_k + B^+ Bref u_k

    // The first term makes the open-loop poles that of the reference
    // system, and the second term makes the input behave like that of the
    // reference system.
    m_A = discB.solve(discA.minus(discAref)).times(-1.0);
    m_B = discB.solve(discBref);

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
   * @param i Row of u.
   * @return The row of the control input vector.
   */
  public double getU(int i) {
    return m_u.get(i, 0);
  }

  /** Resets the controller. */
  public void reset() {
    m_u.fill(0.0);
  }

  /**
   * Returns the next output of the controller.
   *
   * @param x The current state x.
   * @param u The current input for the original model.
   * @return The next controller output.
   */
  public Matrix<Inputs, N1> calculate(Matrix<States, N1> x, Matrix<Inputs, N1> u) {
    m_u = m_A.times(x).plus(m_B.times(u));
    return m_u;
  }
}
