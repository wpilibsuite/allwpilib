// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * A measure holds the magnitude and unit of some dimension, such as distance, time, or speed. An
 * immutable measure is <i>immutable</i> and <i>type safe</i>, making it easy to use in concurrent
 * situations and gives compile-time safety. Two measures with the same <i>unit</i> and
 * <i>magnitude</i> are effectively equivalent objects.
 *
 * @param <U> the unit type of the measure
 */
public record ImmutableMeasure<U extends Unit>(double magnitude, double baseUnitMagnitude, U unit)
    implements Measure<U> {
  /**
   * Creates a new measure in the given unit with a magnitude equal to the given one in base units.
   *
   * @param <U> the type of the units of measure
   * @param baseUnitMagnitude the magnitude of the measure, in terms of the base unit of measure
   * @param unit the unit of measure
   * @return a new measure
   */
  public static <U extends Unit> ImmutableMeasure<U> ofBaseUnits(double baseUnitMagnitude, U unit) {
    return new ImmutableMeasure<>(unit.fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, unit);
  }

  /**
   * Creates a new measure in the given unit with a magnitude in terms of that unit.
   *
   * @param <U> the type of the units of measure
   * @param relativeMagnitude the magnitude of the measure
   * @param unit the unit of measure
   * @return a new measure
   */
  public static <U extends Unit> ImmutableMeasure<U> ofRelativeUnits(
      double relativeMagnitude, U unit) {
    return new ImmutableMeasure<>(relativeMagnitude, unit.toBaseUnits(relativeMagnitude), unit);
  }

  @Override
  public Measure<U> copy() {
    return this; // already immutable, no need to allocate a new object
  }
}
