// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import org.wpilib.math.jni.WPIMathJNI;

/** Variable JNI functions. */
final class VariableMatrixJNI extends WPIMathJNI {
  private VariableMatrixJNI() {
    // Utility class.
  }

  /**
   * Solves the VariableMatrix equation AX = B for X.
   *
   * @param A The left-hand side as a flattened row-major matrix.
   * @param Acols The number of columns in A.
   * @param B The right-hand side as a flattened row-major matrix.
   * @param Bcols The number of columns in B.
   * @return The solution X as a flattened row-major matrix.
   */
  static native long[] solve(long[] A, int Acols, long[] B, int Bcols);
}
