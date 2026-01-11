// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import org.wpilib.math.jni.WPIMathJNI;

/** Variable JNI functions. */
final class VariableJNI extends WPIMathJNI {
  private VariableJNI() {
    // Utility class.
  }

  /** Constructs a default Variable. */
  static native long createDefault();

  /**
   * Constructs a Variable from a floating point type.
   *
   * @param value The value of the Variable.
   */
  static native long createDouble(double value);

  /**
   * Constructs a Variable from an integral type.
   *
   * @param value The value of the Variable.
   */
  static native long createInt(int value);

  /**
   * Destructs a Variable.
   *
   * @param handle Variable handle.
   */
  static native void destroy(long handle);

  /**
   * Sets Variable's internal value.
   *
   * @param handle Variable handle.
   * @param value The value of the Variable.
   */
  static native void setValue(long handle, double value);

  /**
   * Variable-Variable multiplication operator.
   *
   * @param handle Variable handle.
   * @param rhs Operator right-hand side.
   * @return Result of multiplication.
   */
  static native long times(long handle, long rhs);

  /**
   * Variable-Variable division operator.
   *
   * @param handle Variable handle.
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  static native long div(long handle, long rhs);

  /**
   * Variable-Variable addition operator.
   *
   * @param handle Variable handle.
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  static native long plus(long handle, long rhs);

  /**
   * Variable-Variable subtraction operator.
   *
   * @param handle Variable handle.
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  static native long minus(long handle, long rhs);

  /**
   * Unary minus operator.
   *
   * @param handle Variable handle.
   */
  static native long unaryMinus(long handle);

  /**
   * Returns the value of this variable.
   *
   * @param handle Variable handle.
   * @return The value of this variable.
   */
  static native double value(long handle);

  /**
   * Returns the type of this expression (constant, linear, quadratic, or nonlinear).
   *
   * @param handle Variable handle.
   * @return The type of this expression.
   */
  static native int type(long handle);

  /**
   * Math.abs() for Variables.
   *
   * @param x The argument.
   */
  static native long abs(long x);

  /**
   * Math.acos() for Variables.
   *
   * @param x The argument.
   */
  static native long acos(long x);

  /**
   * Math.asin() for Variables.
   *
   * @param x The argument.
   */
  static native long asin(long x);

  /**
   * Math.atan() for Variables.
   *
   * @param x The argument.
   */
  static native long atan(long x);

  /**
   * Math.atan2() for Variables.
   *
   * @param y The y argument.
   * @param x The x argument.
   */
  static native long atan2(long y, long x);

  /**
   * Math.cbrt() for Variables.
   *
   * @param x The argument.
   */
  static native long cbrt(long x);

  /**
   * Math.cos() for Variables.
   *
   * @param x The argument.
   */
  static native long cos(long x);

  /**
   * Math.cosh() for Variables.
   *
   * @param x The argument.
   */
  static native long cosh(long x);

  /**
   * Math.exp() for Variables.
   *
   * @param x The argument.
   */
  static native long exp(long x);

  /**
   * Math.hypot() for Variables.
   *
   * @param x The x argument.
   * @param y The y argument.
   */
  static native long hypot(long x, long y);

  /**
   * Math.pow() for Variables.
   *
   * @param base The base.
   * @param power The power.
   */
  static native long pow(long base, long power);

  /**
   * Math.log() for Variables.
   *
   * @param x The argument.
   */
  static native long log(long x);

  /**
   * Math.log10() for Variables.
   *
   * @param x The argument.
   */
  static native long log10(long x);

  /**
   * Math.signum() for Variables.
   *
   * @param x The argument.
   */
  static native long signum(long x);

  /**
   * Math.sin() for Variables.
   *
   * @param x The argument.
   */
  static native long sin(long x);

  /**
   * Math.sinh() for Variables.
   *
   * @param x The argument.
   */
  static native long sinh(long x);

  /**
   * Math.sqrt() for Variables.
   *
   * @param x The argument.
   */
  static native long sqrt(long x);

  /**
   * Math.tan() for Variables.
   *
   * @param x The argument.
   */
  static native long tan(long x);

  /**
   * Math.tanh() for Variables.
   *
   * @param x The argument.
   */
  static native long tanh(long x);

  /**
   * Returns the total native memory usage of Variables in bytes.
   *
   * @return The total native memory usage of Variables in bytes.
   */
  static native long totalNativeMemoryUsage();
}
