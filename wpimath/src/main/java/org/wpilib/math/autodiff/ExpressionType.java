// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

/**
 * Expression type.
 *
 * <p>Used for autodiff caching.
 */
public enum ExpressionType {
  /** There is no expression. */
  NONE(0),
  /** The expression is a constant. */
  CONSTANT(1),
  /** The expression is composed of linear and lower-order operators. */
  LINEAR(2),
  /** The expression is composed of quadratic and lower-order operators. */
  QUADRATIC(3),
  /** The expression is composed of nonlinear and lower-order operators. */
  NONLINEAR(4);

  /** ExpressionType value. */
  public final int value;

  ExpressionType(int value) {
    this.value = value;
  }

  /**
   * Converts integer to its corresponding enum value.
   *
   * @param x The integer.
   * @return The enum value.
   */
  public static ExpressionType fromInt(int x) {
    return switch (x) {
      case 0 -> ExpressionType.NONE;
      case 1 -> ExpressionType.CONSTANT;
      case 2 -> ExpressionType.LINEAR;
      case 3 -> ExpressionType.QUADRATIC;
      case 4 -> ExpressionType.NONLINEAR;
      default -> null;
    };
  }
}
