// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Unit;

/**
 * An inverse interpolation function which determines where within an interpolation range an object
 * lies. This behavior can be linear or nonlinear.
 *
 * @param <T> The type that the {@link InverseInterpolator} will operate on.
 */
@FunctionalInterface
public interface InverseInterpolator<T> {
  /**
   * Return where within interpolation range [0, 1] q is between startValue and endValue.
   *
   * @param startValue Lower part of interpolation range.
   * @param endValue Upper part of interpolation range.
   * @param q Query.
   * @return Interpolant in range [0, 1].
   */
  double inverseInterpolate(T startValue, T endValue, T q);

  /**
   * Returns inverse interpolator for Double.
   *
   * @return Inverse interpolator for Double.
   */
  static InverseInterpolator<Double> forDouble() {
    return MathUtil::inverseLerp;
  }

  /**
   * Returns inverse interpolator for a Measure.
   *
   * @param <U> The unit of the Measure.
   * @param <M> The type of the Measure.
   * @return Inverse interpolator for a Measure.
   */
  static <U extends Unit, M extends Measure<U>> InverseInterpolator<M> forMeasure() {
    return MathUtil::inverseLerp;
  }
}
