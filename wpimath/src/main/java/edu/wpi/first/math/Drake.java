// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import org.ejml.simple.SimpleMatrix;

public final class Drake {
  private Drake() {}

  /**
   * Solves the discrete alegebraic Riccati equation.
   *
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @return Solution of DARE.
   */
  public static SimpleMatrix discreteAlgebraicRiccatiEquation(
      SimpleMatrix A, SimpleMatrix B, SimpleMatrix Q, SimpleMatrix R) {
    var S = new SimpleMatrix(A.numRows(), A.numCols());
    WPIMathJNI.discreteAlgebraicRiccatiEquation(
        A.getDDRM().getData(),
        B.getDDRM().getData(),
        Q.getDDRM().getData(),
        R.getDDRM().getData(),
        A.numCols(),
        B.numCols(),
        S.getDDRM().getData());
    return S;
  }

  /**
   * Solves the discrete alegebraic Riccati equation.
   *
   * @param <States> Number of states.
   * @param <Inputs> Number of inputs.
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @return Solution of DARE.
   */
  public static <States extends Num, Inputs extends Num>
      Matrix<States, States> discreteAlgebraicRiccatiEquation(
          Matrix<States, States> A,
          Matrix<States, Inputs> B,
          Matrix<States, States> Q,
          Matrix<Inputs, Inputs> R) {
    return new Matrix<>(
        discreteAlgebraicRiccatiEquation(
            A.getStorage(), B.getStorage(), Q.getStorage(), R.getStorage()));
  }

  /**
   * Solves the discrete alegebraic Riccati equation.
   *
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @param N State-input cross-term cost matrix.
   * @return Solution of DARE.
   */
  public static SimpleMatrix discreteAlgebraicRiccatiEquation(
      SimpleMatrix A, SimpleMatrix B, SimpleMatrix Q, SimpleMatrix R, SimpleMatrix N) {
    // See
    // https://en.wikipedia.org/wiki/Linear%E2%80%93quadratic_regulator#Infinite-horizon,_discrete-time_LQR
    // for the change of variables used here.
    var scrA = A.minus(B.mult(R.solve(N.transpose())));
    var scrQ = Q.minus(N.mult(R.solve(N.transpose())));

    var S = new SimpleMatrix(A.numRows(), A.numCols());
    WPIMathJNI.discreteAlgebraicRiccatiEquation(
        scrA.getDDRM().getData(),
        B.getDDRM().getData(),
        scrQ.getDDRM().getData(),
        R.getDDRM().getData(),
        A.numCols(),
        B.numCols(),
        S.getDDRM().getData());
    return S;
  }

  /**
   * Solves the discrete alegebraic Riccati equation.
   *
   * @param <States> Number of states.
   * @param <Inputs> Number of inputs.
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @param N State-input cross-term cost matrix.
   * @return Solution of DARE.
   */
  public static <States extends Num, Inputs extends Num>
      Matrix<States, States> discreteAlgebraicRiccatiEquation(
          Matrix<States, States> A,
          Matrix<States, Inputs> B,
          Matrix<States, States> Q,
          Matrix<Inputs, Inputs> R,
          Matrix<States, Inputs> N) {
    // See
    // https://en.wikipedia.org/wiki/Linear%E2%80%93quadratic_regulator#Infinite-horizon,_discrete-time_LQR
    // for the change of variables used here.
    var scrA = A.minus(B.times(R.solve(N.transpose())));
    var scrQ = Q.minus(N.times(R.solve(N.transpose())));

    return new Matrix<>(
        discreteAlgebraicRiccatiEquation(
            scrA.getStorage(), B.getStorage(), scrQ.getStorage(), R.getStorage()));
  }
}
