// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.jni;

/** Eigen JNI. */
public final class EigenJNI extends WPIMathJNI {
  /**
   * Computes the matrix exp.
   *
   * @param src Array of elements of the matrix to be exponentiated.
   * @param rows How many rows there are.
   * @param dst Array where the result will be stored.
   */
  public static native void exp(double[] src, int rows, double[] dst);

  /**
   * Computes the matrix pow.
   *
   * @param src Array of elements of the matrix to be raised to a power.
   * @param rows How many rows there are.
   * @param exponent The exponent.
   * @param dst Array where the result will be stored.
   */
  public static native void pow(double[] src, int rows, double exponent, double[] dst);

  /** Utility class. */
  private EigenJNI() {}
}
