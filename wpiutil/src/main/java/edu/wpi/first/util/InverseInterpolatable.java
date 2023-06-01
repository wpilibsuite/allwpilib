// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

/**
 * InverseInterpolatable allows an object to determine where within an interpolation range it lies.
 *
 * @param <T> The class that is inverse interpolatable.
 */
public interface InverseInterpolatable<T> {
  /**
   * Return how far between lower and upper bound this object is.
   *
   * @param lower The lower bound of the interpolation range.
   * @param upper The upper bound of the interpolation range.
   * @return Interpolant in range [0, 1].
   */
  double inverseInterpolate(T lower, T upper);
}
