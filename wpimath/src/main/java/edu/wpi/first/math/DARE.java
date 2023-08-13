// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import org.ejml.simple.SimpleMatrix;

public final class DARE {
  private DARE() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Solves the discrete algebraic Riccati equation.
   *
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @return Solution of DARE.
   * @throws IllegalArgumentException if Q isn't symmetric positive semidefinite.
   * @throws IllegalArgumentException if R isn't symmetric positive definite.
   * @throws IllegalArgumentException if the (A, B) pair isn't stabilizable.
   * @throws IllegalArgumentException if the (A, C) pair where Q = CᵀC isn't detectable.
   */
  public static SimpleMatrix dare(SimpleMatrix A, SimpleMatrix B, SimpleMatrix Q, SimpleMatrix R) {
    var S = new SimpleMatrix(A.getNumRows(), A.getNumCols());
    WPIMathJNI.dareABQR(
        A.getDDRM().getData(),
        B.getDDRM().getData(),
        Q.getDDRM().getData(),
        R.getDDRM().getData(),
        A.getNumCols(),
        B.getNumCols(),
        S.getDDRM().getData());
    return S;
  }

  /**
   * Solves the discrete algebraic Riccati equation.
   *
   * @param <States> Number of states.
   * @param <Inputs> Number of inputs.
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @return Solution of DARE.
   * @throws IllegalArgumentException if Q isn't symmetric positive semidefinite.
   * @throws IllegalArgumentException if R isn't symmetric positive definite.
   * @throws IllegalArgumentException if the (A, B) pair isn't stabilizable.
   * @throws IllegalArgumentException if the (A, C) pair where Q = CᵀC isn't detectable.
   */
  public static <States extends Num, Inputs extends Num> Matrix<States, States> dare(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R) {
    return new Matrix<>(dare(A.getStorage(), B.getStorage(), Q.getStorage(), R.getStorage()));
  }

  /**
   * Solves the discrete algebraic Riccati equation.
   *
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @param N State-input cross-term cost matrix.
   * @return Solution of DARE.
   * @throws IllegalArgumentException if Q − NR⁻¹Nᵀ isn't symmetric positive semidefinite.
   * @throws IllegalArgumentException if R isn't symmetric positive definite.
   * @throws IllegalArgumentException if the (A, B) pair isn't stabilizable.
   * @throws IllegalArgumentException if the (A, C) pair where Q = CᵀC isn't detectable.
   */
  public static SimpleMatrix dare(
      SimpleMatrix A, SimpleMatrix B, SimpleMatrix Q, SimpleMatrix R, SimpleMatrix N) {
    var S = new SimpleMatrix(A.getNumRows(), A.getNumCols());
    WPIMathJNI.dareABQRN(
        A.getDDRM().getData(),
        B.getDDRM().getData(),
        Q.getDDRM().getData(),
        R.getDDRM().getData(),
        N.getDDRM().getData(),
        A.getNumCols(),
        B.getNumCols(),
        S.getDDRM().getData());
    return S;
  }

  /**
   * Solves the discrete algebraic Riccati equation.
   *
   * @param <States> Number of states.
   * @param <Inputs> Number of inputs.
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @param N State-input cross-term cost matrix.
   * @return Solution of DARE.
   * @throws IllegalArgumentException if Q − NR⁻¹Nᵀ isn't symmetric positive semidefinite.
   * @throws IllegalArgumentException if R isn't symmetric positive definite.
   * @throws IllegalArgumentException if the (A, B) pair isn't stabilizable.
   * @throws IllegalArgumentException if the (A, C) pair where Q = CᵀC isn't detectable.
   */
  public static <States extends Num, Inputs extends Num> Matrix<States, States> dare(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, Inputs> N) {
    return new Matrix<>(
        dare(A.getStorage(), B.getStorage(), Q.getStorage(), R.getStorage(), N.getStorage()));
  }
}
