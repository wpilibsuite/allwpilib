// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.function;

/**
 * Represents an operation that accepts a single float-valued argument and returns no result. This
 * is the primitive type specialization of {@link java.util.function.Consumer} for float. Unlike
 * most other functional interfaces, BooleanConsumer is expected to operate via side effects.
 *
 * <p>This is a functional interface whose functional method is {@link #accept(float)}.
 */
@FunctionalInterface
public interface FloatConsumer {
  /**
   * Performs this operation on the given argument.
   *
   * @param value the input argument
   */
  void accept(float value);
}
