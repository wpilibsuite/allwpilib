/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

import org.ejml.simple.SimpleMatrix;

public final class Drake {
  private Drake() {
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
  @SuppressWarnings({"LocalVariableName", "ParameterName"})
  public static SimpleMatrix discreteAlgebraicRiccatiEquation(
          SimpleMatrix A,
          SimpleMatrix B,
          SimpleMatrix Q,
          SimpleMatrix R) {
    var S = new SimpleMatrix(A.numRows(), A.numCols());
    DrakeJNI.discreteAlgebraicRiccatiEquation(A.getDDRM().getData(), B.getDDRM().getData(),
            Q.getDDRM().getData(), R.getDDRM().getData(), A.numCols(), B.numCols(),
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
  @SuppressWarnings("ParameterName")
  public static SimpleMatrix discreteAlgebraicRiccatiEquation(
          Matrix A,
          Matrix B,
          Matrix Q,
          Matrix R) {
    return discreteAlgebraicRiccatiEquation(A.getStorage(), B.getStorage(), Q.getStorage(),
            R.getStorage());
  }
}
