// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.system;

import org.wpilib.math.jni.LinearSystemUtilJNI;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.util.Num;

/** Linear system utilities. */
public final class LinearSystemUtil {
  private LinearSystemUtil() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Returns true if (A, B) is a stabilizable pair.
   *
   * <p>(A, B) is stabilizable if and only if the uncontrollable eigenvalues of A, if any, have
   * absolute values less than one, where an eigenvalue is uncontrollable if rank([λI - A, B]) %3C n
   * where n is the number of states.
   *
   * @param <States> Num representing the size of A.
   * @param <Inputs> Num representing the columns of B.
   * @param A System matrix.
   * @param B Input matrix.
   * @return If the system is stabilizable.
   */
  public static <States extends Num, Inputs extends Num> boolean isStabilizable(
      Matrix<States, States> A, Matrix<States, Inputs> B) {
    return LinearSystemUtilJNI.isStabilizable(
        A.getNumRows(), B.getNumCols(), A.getData(), B.getData());
  }

  /**
   * Returns true if (A, C) is a detectable pair.
   *
   * <p>(A, C) is detectable if and only if the unobservable eigenvalues of A, if any, have absolute
   * values less than one, where an eigenvalue is unobservable if rank([λI - A; C]) %3C n where n is
   * the number of states.
   *
   * @param <States> Num representing the size of A.
   * @param <Outputs> Num representing the rows of C.
   * @param A System matrix.
   * @param C Output matrix.
   * @return If the system is detectable.
   */
  public static <States extends Num, Outputs extends Num> boolean isDetectable(
      Matrix<States, States> A, Matrix<Outputs, States> C) {
    return LinearSystemUtilJNI.isStabilizable(
        A.getNumRows(), C.getNumRows(), A.transpose().getData(), C.transpose().getData());
  }
}
