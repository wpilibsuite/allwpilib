// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.autodiff;

import edu.wpi.first.math.jni.WPIMathJNI;
import org.ejml.simple.SimpleMatrix;

/** Hessian JNI functions. */
public final class HessianJNI extends WPIMathJNI {
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
   * Returns the Hessian as a VariableMatrix.
   *
   * <p>This is useful when constructing optimization problems with derivatives in them.
   *
   * @param handle Hessian handle.
   * @return The Hessian as a VariableMatrix.
   */
  static native VariableMatrix get(long handle);

  /**
   * Evaluates the Hessian at wrt's value.
   *
   * @param handle Hessian handle.
   * @return The Hessian at wrt's value.
   */
  static native SimpleMatrix value(long handle);
}
