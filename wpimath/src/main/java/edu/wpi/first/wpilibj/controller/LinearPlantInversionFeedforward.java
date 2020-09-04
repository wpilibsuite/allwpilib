/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.wpilibj.math.Discretization;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

/**
 * Constructs a plant inversion model-based feedforward from a {@link LinearSystem}.
 *
 * <p>The feedforward is calculated as <strong> u_ff = B<sup>+</sup> (r_k+1 - A r_k) </strong>,
 * where <strong> B<sup>+</sup> </strong> is the pseudoinverse of B.
 *
 * <p>For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 */
@SuppressWarnings({"ParameterName", "LocalVariableName", "MemberName", "ClassTypeParameterName"})
public class LinearPlantInversionFeedforward<States extends Num, Inputs extends Num,
        Outputs extends Num> {
  /**
   * The current reference state.
   */
  @SuppressWarnings("MemberName")
  private Matrix<States, N1> m_r;

  /**
   * The computed feedforward.
   */
  private Matrix<Inputs, N1> m_uff;

  @SuppressWarnings("MemberName")
  private final Matrix<States, Inputs> m_B;

  @SuppressWarnings("MemberName")
  private final Matrix<States, States> m_A;

  /**
   * Constructs a feedforward with the given plant.
   *
   * @param plant     The plant being controlled.
   * @param dtSeconds Discretization timestep.
   */
  public LinearPlantInversionFeedforward(
          LinearSystem<States, Inputs, Outputs> plant,
          double dtSeconds
  ) {
    this(plant.getA(), plant.getB(), dtSeconds);
  }

  /**
   * Constructs a feedforward with the given coefficients.
   *
   * @param A         Continuous system matrix of the plant being controlled.
   * @param B         Continuous input matrix of the plant being controlled.
   * @param dtSeconds Discretization timestep.
   */
  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  public LinearPlantInversionFeedforward(Matrix<States, States> A, Matrix<States, Inputs> B,
                                         double dtSeconds) {
    var discABPair = Discretization.discretizeAB(A, B, dtSeconds);
    this.m_A = discABPair.getFirst();
    this.m_B = discABPair.getSecond();

    m_r = new Matrix<>(new SimpleMatrix(B.getNumRows(), 1));
    m_uff = new Matrix<>(new SimpleMatrix(B.getNumCols(), 1));

    reset(m_r);
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
   *
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
   *
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
  public void reset(Matrix<States, N1> initialState) {
    m_r = initialState;
    m_uff.fill(0.0);
  }

  /**
   * Resets the feedforward with a zero initial state vector.
   */
  public void reset() {
    m_r.fill(0.0);
    m_uff.fill(0.0);
  }

  /**
   * Calculate the feedforward with only the desired
   * future reference. This uses the internally stored "current"
   * reference.
   *
   * <p>If this method is used the initial state of the system is the one
   * set using {@link LinearPlantInversionFeedforward#reset(Matrix)}.
   * If the initial state is not set it defaults to a zero vector.
   *
   * @param nextR The reference state of the future timestep (k + dt).
   *
   * @return The calculated feedforward.
   */
  public Matrix<Inputs, N1> calculate(Matrix<States, N1> nextR) {
    return calculate(m_r, nextR);
  }

  /**
   * Calculate the feedforward with current and future reference vectors.
   *
   * @param r     The reference state of the current timestep (k).
   * @param nextR The reference state of the future timestep (k + dt).
   *
   * @return The calculated feedforward.
   */
  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  public Matrix<Inputs, N1> calculate(Matrix<States, N1> r, Matrix<States, N1> nextR) {
    m_uff = new Matrix<>(m_B.solve(nextR.minus(m_A.times(r))));

    m_r = nextR;
    return m_uff;
  }
}
