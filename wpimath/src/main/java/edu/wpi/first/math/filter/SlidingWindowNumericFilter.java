// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

/**
 * Interface for sliding window filters that operate by repeatedly taking in new values and
 * returning what the current value should be as modulated through the filter.
 */
public interface SlidingWindowNumericFilter {
  /** Resets the filter to initial conditions. */
  void reset();

  /**
   * Calculates the next value of the filter.
   *
   * @param input Current input value.
   * @return The filtered value at this step.
   */
  double calculate(double input);
}
