// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.autodiff;

import edu.wpi.first.math.jni.WPIMathJNI;
import org.ejml.simple.SimpleMatrix;

/** Gradient JNI functions. */
public final class GradientJNI extends WPIMathJNI {
  private GradientJNI() {
    // Utility class.
  }

  /**
   * Constructs a Gradient object.
   *
   * @param variable Variable of which to compute the Gradient.
   * @param wrt Vector of variables with respect to which to compute the Gradient.
   */
  static native long create(long variable, long[] wrt);

  /**
   * Destructs a Gradient.
   *
   * @param handle Gradient handle.
   */
  static native void destroy(long handle);

  /**
   * Returns the Gradient as a VariableMatrix.
   *
   * <p>This is useful when constructing optimization problems with derivatives in them.
   *
   * @param handle Gradient handle.
   * @return The Gradient as a VariableMatrix.
   */
  static native VariableMatrix get(long handle);

  /**
   * Evaluates the Gradient at wrt's value.
   *
   * @param handle Gradient handle.
   * @return The Gradient at wrt's value.
   */
  static native SimpleMatrix value(long handle);
}
