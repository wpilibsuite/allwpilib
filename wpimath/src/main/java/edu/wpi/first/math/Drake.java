/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.math;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;

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
    WPIMathJNI.discreteAlgebraicRiccatiEquation(A.getDDRM().getData(), B.getDDRM().getData(),
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
  @SuppressWarnings({"ParameterName", "MethodTypeParameterName"})
  public static <States extends Num, Inputs extends Num> Matrix<States, States>
      discreteAlgebraicRiccatiEquation(Matrix<States, States> A,
                                       Matrix<States, Inputs> B,
                                       Matrix<States, States> Q,
                                       Matrix<Inputs, Inputs> R) {
    return new Matrix<>(discreteAlgebraicRiccatiEquation(A.getStorage(), B.getStorage(),
    Q.getStorage(), R.getStorage()));
  }
}
