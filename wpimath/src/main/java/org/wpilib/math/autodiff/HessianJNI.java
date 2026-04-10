// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import org.wpilib.math.jni.WPIMathJNI;

/** Hessian JNI functions. */
final class HessianJNI extends WPIMathJNI {
  private HessianJNI() {
    // Utility class.
  }

  /**
   * Constructs a Hessian object.
   *
   * @param variable Variable of which to compute the Hessian.
   * @param wrt Vector of variables with respect to which to compute the Hessian.
   */
  static native long create(long variable, long[] wrt);

  /**
   * Destructs a Hessian.
   *
   * @param handle Hessian handle.
   */
  static native void destroy(long handle);

  /**
   * Returns the Hessian as an array of Variable handles.
   *
   * <p>This is useful when constructing optimization problems with derivatives in them.
   *
   * @param handle Hessian handle.
   * @return The Hessian as an array of Variable handles.
   */
  static native long[] get(long handle);

  /**
   * Evaluates the Hessian at wrt's value.
   *
   * @param handle Hessian handle.
   * @return A record containing the triplet row, column and value arrays (int[], int[], and
   *     double[] respectively).
   */
  static native NativeSparseTriplets value(long handle);
}
