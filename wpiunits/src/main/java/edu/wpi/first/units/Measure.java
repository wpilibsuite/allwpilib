// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * A measure holds the magnitude and unit of some dimension, such as distance, time, or speed. Two
 * measures with the same <i>unit</i> and <i>magnitude</i> are effectively equivalent objects.
 *
 * @param <U> the unit type of the measure
 */
public interface Measure<U extends Unit> extends Comparable<Measure<U>> {
  /**
   * The threshold for two measures to be considered equivalent if converted to the same unit. This
   * is only needed due to floating-point error.
   */
  double EQUIVALENCE_THRESHOLD = 1e-12;

  /**
   * Gets the unitless magnitude of this measure.
   *
   * @return the magnitude in terms of {@link #unit() the unit}.
   */
  double magnitude();

  /**
   * Gets the magnitude of this measure in terms of the base unit. If the unit is the base unit for
   * its system of measure, then the value will be equivalent to {@link #magnitude()}.
   *
   * @return the magnitude in terms of the base unit
   */
  double baseUnitMagnitude();

  /**
   * Gets the units of this measure.
   *
   * @return the unit
   */
  U unit();

  /**
   * Converts this measure to a measure with a different unit of the same type, eg minutes to
   * seconds. Converting to the same unit is equivalent to calling {@link #magnitude()}.
   *
   * <pre>
   *   Meters.of(12).in(Feet) // 39.3701
   *   Seconds.of(15).in(Minutes) // 0.25
   * </pre>
   *
   * @param unit the unit to convert this measure to
   * @return the value of this measure in the given unit
   */
  default double in(U unit) {
    if (this.unit().equals(unit)) {
      return magnitude();
    } else {
      return unit.fromBaseUnits(baseUnitMagnitude());
    }
  }

  @SuppressWarnings("unchecked")
  default U baseUnit() {
    return (U) unit().getBaseUnit();
  }

  /**
   * Returns an immutable copy of this measure. The copy can be used freely and is guaranteed never
   * to change.
   *
   * @return the copied measure
   */
  Measure<U> copy();

  /**
   * Checks if this measure is near another measure of the same unit. Provide a variance threshold
   * for use for a +/- scalar, such as 0.05 for +/- 5%.
   *
   * <pre>
   *   Inches.of(11).isNear(Inches.of(10), 0.1) // true
   *   Inches.of(12).isNear(Inches.of(10), 0.1) // false
   * </pre>
   *
   * @param other the other measurement to compare against
   * @param varianceThreshold the acceptable variance threshold, in terms of an acceptable +/- error
   *     range multiplier. Checking if a value is within 10% means a value of 0.1 should be passed;
   *     checking if a value is within 1% means a value of 0.01 should be passed, and so on.
   * @return true if this unit is near the other measure, otherwise false
   */
  default boolean isNear(Measure<?> other, double varianceThreshold) {
    if (!this.unit().getBaseUnit().equivalent(other.unit().getBaseUnit())) {
      return false; // Disjoint units, not compatible
    }

    // abs so negative inputs are calculated correctly
    var tolerance = Math.abs(other.baseUnitMagnitude() * varianceThreshold);

    return Math.abs(this.baseUnitMagnitude() - other.baseUnitMagnitude()) <= tolerance;
  }

  /**
   * Checks if this measure is near another measure of the same unit, with a specified tolerance of
   * the same unit.
   *
   * <pre>
   *     Meters.of(1).isNear(Meters.of(1.2), Millimeters.of(300)) // true
   *     Degrees.of(90).isNear(Rotations.of(0.5), Degrees.of(45)) // false
   * </pre>
   *
   * @param other the other measure to compare against.
   * @param tolerance the tolerance allowed in which the two measures are defined as near each
   *     other.
   * @return true if this unit is near the other measure, otherwise false.
   */
  default boolean isNear(Measure<U> other, Measure<U> tolerance) {
    return Math.abs(this.baseUnitMagnitude() - other.baseUnitMagnitude())
        <= Math.abs(tolerance.baseUnitMagnitude());
  }

  /**
   * Checks if this measure is equivalent to another measure of the same unit.
   *
   * @param other the measure to compare to
   * @return true if this measure is equivalent, false otherwise
   */
  default boolean isEquivalent(Measure<?> other) {
    // Return false for disjoint units that aren't compatible
    return this.unit().getBaseUnit().equals(other.unit().getBaseUnit())
        && Math.abs(baseUnitMagnitude() - other.baseUnitMagnitude()) <= EQUIVALENCE_THRESHOLD;
  }

  /** {@inheritDoc} */
  @Override
  default int compareTo(Measure<U> o) {
    return Double.compare(this.baseUnitMagnitude(), o.baseUnitMagnitude());
  }

  /**
   * Checks if this measure is greater than another measure of the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has a greater equivalent magnitude, false otherwise
   */
  default boolean gt(Measure<U> o) {
    return compareTo(o) > 0;
  }

  /**
   * Checks if this measure is greater than or equivalent to another measure of the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has an equal or greater equivalent magnitude, false otherwise
   */
  default boolean gte(Measure<U> o) {
    return compareTo(o) > 0 || isEquivalent(o);
  }

  /**
   * Checks if this measure is less than another measure of the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has a lesser equivalent magnitude, false otherwise
   */
  default boolean lt(Measure<U> o) {
    return compareTo(o) < 0;
  }

  /**
   * Checks if this measure is less than or equivalent to another measure of the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has an equal or lesser equivalent magnitude, false otherwise
   */
  default boolean lte(Measure<U> o) {
    return compareTo(o) < 0 || isEquivalent(o);
  }

  /**
   * Returns the measure with the absolute value closest to positive infinity.
   *
   * @param <U> the type of the units of the measures
   * @param measures the set of measures to compare
   * @return the measure with the greatest positive magnitude, or null if no measures were provided
   */
  @SafeVarargs
  static <U extends Unit> Measure<U> max(Measure<U>... measures) {
    if (measures.length == 0) {
      return null; // nothing to compare
    }

    Measure<U> max = null;
    for (Measure<U> measure : measures) {
      if (max == null || measure.gt(max)) {
        max = measure;
      }
    }

    return max;
  }

  /**
   * Returns the measure with the absolute value closest to negative infinity.
   *
   * @param <U> the type of the units of the measures
   * @param measures the set of measures to compare
   * @return the measure with the greatest negative magnitude
   */
  @SafeVarargs
  static <U extends Unit> Measure<U> min(Measure<U>... measures) {
    if (measures.length == 0) {
      return null; // nothing to compare
    }

    Measure<U> max = null;
    for (Measure<U> measure : measures) {
      if (max == null || measure.lt(max)) {
        max = measure;
      }
    }

    return max;
  }

  /**
   * Returns a string representation of this measurement in a shorthand form. The symbol of the
   * backing unit is used, rather than the full name, and the magnitude is represented in scientific
   * notation.
   *
   * @return the short form representation of this measurement
   */
  default String toShortString() {
    // eg 1.234e+04 V/m (1234 Volt per Meter in long form)
    return String.format("%.3e %s", magnitude(), unit().symbol());
  }

  /**
   * Returns a string representation of this measurement in a longhand form. The name of the backing
   * unit is used, rather than its symbol, and the magnitude is represented in a full string, not
   * scientific notation. (Very large values may be represented in scientific notation, however)
   *
   * @return the long form representation of this measurement
   */
  default String toLongString() {
    // eg 1234 Volt per Meter (1.234e+04 V/m in short form)
    return String.format("%s %s", magnitude(), unit().name());
  }
}
