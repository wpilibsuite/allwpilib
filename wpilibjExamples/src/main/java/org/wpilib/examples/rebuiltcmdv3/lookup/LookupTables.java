// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.lookup;

import static org.wpilib.units.Units.Value;

import org.wpilib.math.interpolation.Interpolator;
import org.wpilib.math.interpolation.InverseInterpolator;
import org.wpilib.units.Measure;
import org.wpilib.units.Unit;
import org.wpilib.units.measure.Dimensionless;

/** Utility class for working with unit-based interpolating tree maps. */
public final class LookupTables {
  private LookupTables() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Creates an interpolator operating on Measure objects.
   *
   * @param <U> The type of the unit for the measure
   * @param <M> The type of the measure
   * @return The interpolator for the measure
   */
  @SuppressWarnings("unchecked")
  public static <U extends Unit, M extends Measure<U>> Interpolator<M> unitInterpolator() {
    return (startValue, endValue, t) ->
        (M) endValue.minus(startValue).times(Math.clamp(t, 0, 1)).plus(startValue);
  }

  /**
   * Creates an inverse interpolator operating on Measure objects.
   *
   * @param <U> The type of the unit for the measure
   * @param <M> The type of the measure
   * @return The inverse interpolator for the measure
   */
  public static <U extends Unit, M extends Measure<U>>
      InverseInterpolator<M> inverseUnitInterpolator() {
    return (startValue, endValue, q) -> {
      Measure<U> totalRange = endValue.minus(startValue);
      if (totalRange.baseUnitMagnitude() <= 0) {
        return 0;
      }

      Measure<U> queryToStart = q.minus(startValue);
      if (queryToStart.baseUnitMagnitude() <= 0) {
        return 0;
      }

      return ((Dimensionless) queryToStart.div(totalRange)).in(Value);
    };
  }
}
