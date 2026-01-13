// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.interpolation;

import org.wpilib.math.util.MathUtil;
import org.wpilib.units.Measure;
import org.wpilib.units.Unit;

/**
 * An interpolation function that returns a value interpolated between an upper and lower bound.
 * This behavior can be linear or nonlinear.
 *
 * @param <T> The type that the {@link Interpolator} will operate on.
 */
@FunctionalInterface
public interface Interpolator<T> {
  /**
   * Perform interpolation between two values.
   *
   * @param startValue The value to start at.
   * @param endValue The value to end at.
   * @param t How far between the two values to interpolate. This should be bounded to [0, 1].
   * @return The interpolated value.
   */
  T interpolate(T startValue, T endValue, double t);

  /**
   * Returns interpolator for Double.
   *
   * @return Interpolator for Double.
   */
  static Interpolator<Double> forDouble() {
    return MathUtil::lerp;
  }

  /**
   * Returns interpolator for a Measure.
   *
   * @param <U> The unit of the Measure.
   * @param <M> The type of the Measure.
   * @return Interpolator for a Measure.
   */
  static <U extends Unit, M extends Measure<U>> Interpolator<M> forMeasure() {
    return MathUtil::lerp;
  }
}
