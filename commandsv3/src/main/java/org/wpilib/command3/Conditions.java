// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import java.util.function.BooleanSupplier;

/**
 * Utility class for composing BooleanSupplier conditions with logical operations.
 *
 * <p>This class provides static methods for combining multiple boolean conditions using logical
 * AND, OR, and NOT operations. It is commonly used for creating complex trigger conditions and
 * context compositions.
 */
public final class Conditions {
  private Conditions() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Creates a BooleanSupplier that returns true when both input conditions are true.
   *
   * @param a the first condition
   * @param b the second condition
   * @return a BooleanSupplier that performs logical AND on the two conditions
   */
  public static BooleanSupplier and(BooleanSupplier a, BooleanSupplier b) {
    return () -> a.getAsBoolean() && b.getAsBoolean();
  }

  /**
   * Creates a BooleanSupplier that returns true when either input condition is true.
   *
   * @param a the first condition
   * @param b the second condition
   * @return a BooleanSupplier that performs logical OR on the two conditions
   */
  public static BooleanSupplier or(BooleanSupplier a, BooleanSupplier b) {
    return () -> a.getAsBoolean() || b.getAsBoolean();
  }

  /**
   * Creates a BooleanSupplier that returns the opposite of the input condition.
   *
   * @param a the condition to negate
   * @return a BooleanSupplier that performs logical NOT on the condition
   */
  public static BooleanSupplier not(BooleanSupplier a) {
    return () -> !a.getAsBoolean();
  }
}
