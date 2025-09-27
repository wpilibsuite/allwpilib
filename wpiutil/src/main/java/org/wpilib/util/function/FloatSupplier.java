// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.function;

/**
 * Represents a supplier of float-valued results.
 *
 * <p>This is the float-producing primitive specialization of {@link java.util.function.Supplier}.
 *
 * <p>There is no requirement that a distinct result be returned each time the supplier is invoked.
 *
 * <p>This is a functional interface whose functional method is {@link #getAsFloat()}.
 */
@FunctionalInterface
public interface FloatSupplier {
  /**
   * Gets a result.
   *
   * @return a result
   */
  float getAsFloat();
}
