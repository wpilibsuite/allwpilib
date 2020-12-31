// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;
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
  @SuppressWarnings({"LocalVariableName", "ParameterName"})
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
   * @param A System matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @return Solution of DARE.
   */
  @SuppressWarnings({"ParameterName", "MethodTypeParameterName"})
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
}
