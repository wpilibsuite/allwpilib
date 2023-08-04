// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.numbers.N1;

public class LinearSystem<States extends Num, Inputs extends Num, Outputs extends Num> {
  /** Continuous system matrix. */
  private final Matrix<States, States> m_A;

  /** Continuous input matrix. */
  private final Matrix<States, Inputs> m_B;

  /** Output matrix. */
  private final Matrix<Outputs, States> m_C;

  /** Feedthrough matrix. */
  private final Matrix<Outputs, Inputs> m_D;

  /**
   * Construct a new LinearSystem from the four system matrices.
   *
   * @param A The system matrix A.
   * @param B The input matrix B.
   * @param C The output matrix C.
   * @param D The feedthrough matrix D.
   * @throws IllegalArgumentException if any matrix element isn't finite.
   */
  public LinearSystem(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<Outputs, States> C,
      Matrix<Outputs, Inputs> D) {
    for (int row = 0; row < A.getNumRows(); ++row) {
      for (int col = 0; col < A.getNumCols(); ++col) {
        if (!Double.isFinite(A.get(row, col))) {
          throw new IllegalArgumentException(
              "Elements of A aren't finite. This is usually due to model implementation errors.");
        }
      }
    }
    for (int row = 0; row < B.getNumRows(); ++row) {
      for (int col = 0; col < B.getNumCols(); ++col) {
        if (!Double.isFinite(B.get(row, col))) {
          throw new IllegalArgumentException(
              "Elements of B aren't finite. This is usually due to model implementation errors.");
        }
      }
    }
    for (int row = 0; row < C.getNumRows(); ++row) {
      for (int col = 0; col < C.getNumCols(); ++col) {
        if (!Double.isFinite(C.get(row, col))) {
          throw new IllegalArgumentException(
              "Elements of C aren't finite. This is usually due to model implementation errors.");
        }
      }
    }
    for (int row = 0; row < D.getNumRows(); ++row) {
      for (int col = 0; col < D.getNumCols(); ++col) {
        if (!Double.isFinite(D.get(row, col))) {
          throw new IllegalArgumentException(
              "Elements of D aren't finite. This is usually due to model implementation errors.");
        }
      }
    }

    this.m_A = A;
    this.m_B = B;
    this.m_C = C;
    this.m_D = D;
  }

  /**
   * Returns the system matrix A.
   *
   * @return the system matrix A.
   */
  public Matrix<States, States> getA() {
    return m_A;
  }

  /**
   * Returns an element of the system matrix A.
   *
   * @param row Row of A.
   * @param col Column of A.
   * @return the system matrix A at (i, j).
   */
  public double getA(int row, int col) {
    return m_A.get(row, col);
  }

  /**
   * Returns the input matrix B.
   *
   * @return the input matrix B.
   */
  public Matrix<States, Inputs> getB() {
    return m_B;
  }

  /**
   * Returns an element of the input matrix B.
   *
   * @param row Row of B.
   * @param col Column of B.
   * @return The value of the input matrix B at (i, j).
   */
  public double getB(int row, int col) {
    return m_B.get(row, col);
  }

  /**
   * Returns the output matrix C.
   *
   * @return Output matrix C.
   */
  public Matrix<Outputs, States> getC() {
    return m_C;
  }

  /**
   * Returns an element of the output matrix C.
   *
   * @param row Row of C.
   * @param col Column of C.
   * @return the double value of C at the given position.
   */
  public double getC(int row, int col) {
    return m_C.get(row, col);
  }

  /**
   * Returns the feedthrough matrix D.
   *
   * @return the feedthrough matrix D.
   */
  public Matrix<Outputs, Inputs> getD() {
    return m_D;
  }

  /**
   * Returns an element of the feedthrough matrix D.
   *
   * @param row Row of D.
   * @param col Column of D.
   * @return The feedthrough matrix D at (i, j).
   */
  public double getD(int row, int col) {
    return m_D.get(row, col);
  }

  /**
   * Computes the new x given the old x and the control input.
   *
   * <p>This is used by state observers directly to run updates based on state estimate.
   *
   * @param x The current state.
   * @param clampedU The control input.
   * @param dtSeconds Timestep for model update.
   * @return the updated x.
   */
  public Matrix<States, N1> calculateX(
      Matrix<States, N1> x, Matrix<Inputs, N1> clampedU, double dtSeconds) {
    var discABpair = Discretization.discretizeAB(m_A, m_B, dtSeconds);

    return (discABpair.getFirst().times(x)).plus(discABpair.getSecond().times(clampedU));
  }

  /**
   * Computes the new y given the control input.
   *
   * <p>This is used by state observers directly to run updates based on state estimate.
   *
   * @param x The current state.
   * @param clampedU The control input.
   * @return the updated output matrix Y.
   */
  public Matrix<Outputs, N1> calculateY(Matrix<States, N1> x, Matrix<Inputs, N1> clampedU) {
    return m_C.times(x).plus(m_D.times(clampedU));
  }

  @Override
  public String toString() {
    return String.format(
        "Linear System: A\n%s\n\nB:\n%s\n\nC:\n%s\n\nD:\n%s\n",
        m_A.toString(), m_B.toString(), m_C.toString(), m_D.toString());
  }
}
