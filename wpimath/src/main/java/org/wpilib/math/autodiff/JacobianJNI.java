// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import org.wpilib.math.jni.WPIMathJNI;

/** Jacobian JNI functions. */
final class JacobianJNI extends WPIMathJNI {
  private JacobianJNI() {
    // Utility class.
  }

  /**
   * Constructs a Jacobian object.
   *
   * @param variables Vector of variables of which to compute the Jacobian.
   * @param wrt Vector of variables with respect to which to compute the Jacobian.
   */
  static native long create(long[] variables, long[] wrt);

  /**
   * Destructs a Jacobian.
   *
   * @param handle Jacobian handle.
   */
  static native void destroy(long handle);

  /**
   * Returns the Jacobian as an array of Variable handles.
   *
   * <p>This is useful when constructing optimization problems with derivatives in them.
   *
   * @param handle Jacobian handle.
   * @return The Jacobian as an array of Variable handles.
   */
  static native long[] get(long handle);

  /**
   * Evaluates the Jacobian at wrt's value.
   *
   * @param handle Jacobian handle.
   * @return A record containing the triplet row, column and value arrays (int[], int[], and
   *     double[] respectively).
   */
  static native NativeSparseTriplets value(long handle);
}
