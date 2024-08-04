// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.time;

import java.util.Objects;

/**
 * A measure holds the magnitude and unit of some dimension, such as distance,
 * time, or speed. An
 * immutable measure is <i>immutable</i> and <i>type safe</i>, making it easy to
 * use in concurrent
 * situations and gives compile-time safety. Two measures with the same
 * <i>unit</i> and
 * <i>magnitude</i> are effectively equivalent objects.
 *
 */
public class Time implements Comparable<Time> {

  /**
   * The threshold for two measures to be considered equivalent if converted to
   * the same unit. This
   * is only needed due to floating-point error.
   */
  static double EQUIVALENCE_THRESHOLD = 1e-12;

  protected double m_magnitude;
  protected double m_baseUnitMagnitude;
  protected TimeUnit m_unit;

  /**
   * Creates a new immutable Dimensionless measure instance. This shouldn't be used
   * directly; prefer one of the
   * factory methods instead.
   *
   * @param magnitude the magnitude of this measure
   * @param unit      the unit of this measure.
   */
  Time(double magnitude, double baseUnitMagnitude, TimeUnit unit) {
    Objects.requireNonNull(unit, "Unit cannot be null");
    m_magnitude = magnitude;
    m_baseUnitMagnitude = baseUnitMagnitude;
    m_unit = unit;
  }

  /**
   * Gets the unitless magnitude of this measure.
   *
   * @return the magnitude in terms of {@link #unit() the unit}.
   */
  public double magnitude() {
    return m_magnitude;
  }

  /**
   * Gets the magnitude of this measure in terms of the base unit. If the unit is
   * the base unit for
   * its system of measure, then the value will be equivalent to
   * {@link #magnitude()}.
   *
   * @return the magnitude in terms of the base unit
   */
  public double baseUnitMagnitude() {
    return m_baseUnitMagnitude;
  }

  /**
   * Gets the units of this measure.
   *
   * @return the unit
   */
  public TimeUnit unit() {
    return m_unit;
  }

  /**
   * Converts this measure to a measure with a different unit of the same type, eg
   * minutes to
   * seconds. Converting to the same unit is equivalent to calling
   * {@link #magnitude()}.
   *
   * <pre>
   *   Meters.of(12).in(Feet) // 39.3701
   *   Seconds.of(15).in(Minutes) // 0.25
   * </pre>
   *
   * @param unit the unit to convert this measure to
   * @return the value of this measure in the given unit
   */
  public double in(TimeUnit unit) {
    if (this.unit().equals(unit)) {
      return magnitude();
    } else {
      return unit.fromBaseUnits(baseUnitMagnitude());
    }
  }

  /**
   * Multiplies this measurement by some constant multiplier and returns the
   * result. The magnitude
   * of the result will be the <i>base</i> magnitude multiplied by the scalar
   * value. If the measure
   * uses a unit with a non-linear relation to its base unit (such as Fahrenheit
   * for temperature),
   * then the result will only be a multiple <i>in terms of the base unit</i>.
   *
   * @param multiplier the constant to multiply by
   * @return the resulting measure
   */
  public Time times(double multiplier) {
    return Time.ofBaseUnits(baseUnitMagnitude() * multiplier, unit());
  }

  /**
   * Divides this measurement by some constant divisor and returns the result.
   * This is equivalent to
   * {@code times(1 / divisor)}
   *
   * @param divisor the constant to divide by
   * @return the resulting measure
   * @see #times(double)
   */
  public Time divide(double divisor) {
    return times(1 / divisor);
  }

  /**
   * Adds another measure to this one. The resulting measure has the same unit as
   * this one.
   *
   * @param other the measure to add to this one
   * @return a new measure containing the result
   */
  public Time plus(Time other) {
    return unit().ofBaseUnits(baseUnitMagnitude() + other.baseUnitMagnitude());
  }

  /**
   * Subtracts another measure from this one. The resulting measure has the same
   * unit as this one.
   *
   * @param other the measure to subtract from this one
   * @return a new measure containing the result
   */
  public Time minus(Time other) {
    return unit().ofBaseUnits(baseUnitMagnitude() - other.baseUnitMagnitude());
  }

  /**
   * Negates this measure and returns the result.
   *
   * @return the resulting measure
   */
  public Time negate() {
    return times(-1);
  }

  /**
   * Returns an immutable copy of this measure. The copy can be used freely and is
   * guaranteed never
   * to change.
   *
   * @return the copied measure
   */
  public Time copy() {
    return new Time(m_magnitude, m_baseUnitMagnitude, m_unit);
  }

  /**
   * Creates a new mutable copy of this measure.
   *
   * @return a mutable measure initialized to be identical to this measure
   */
  public MutableTime mutableCopy() {
    return MutableTime.mutable(this);
  }

  /**
   * Checks if this measure is near another measure of the same unit. Provide a
   * variance threshold
   * for use for a +/- scalar, such as 0.05 for +/- 5%.
   *
   * <pre>
   *   Inches.of(11).isNear(Inches.of(10), 0.1) // true
   *   Inches.of(12).isNear(Inches.of(10), 0.1) // false
   * </pre>
   *
   * @param other             the other measurement to compare against
   * @param varianceThreshold the acceptable variance threshold, in terms of an
   *                          acceptable +/- error
   *                          range multiplier. Checking if a value is within 10%
   *                          means a value of 0.1 should be passed;
   *                          checking if a value is within 1% means a value of
   *                          0.01 should be passed, and so on.
   * @return true if this unit is near the other measure, otherwise false
   */
  public boolean isNear(Time other, double varianceThreshold) {
    // abs so negative inputs are calculated correctly
    var tolerance = Math.abs(other.baseUnitMagnitude() * varianceThreshold);

    return Math.abs(this.baseUnitMagnitude() - other.baseUnitMagnitude()) <= tolerance;
  }

  /**
   * Checks if this measure is near another measure of the same unit, with a
   * specified tolerance of
   * the same unit.
   *
   * <pre>
   *     Meters.of(1).isNear(Meters.of(1.2), Millimeters.of(300)) // true
   *     Degrees.of(90).isNear(Rotations.of(0.5), Degrees.of(45)) // false
   * </pre>
   *
   * @param other     the other measure to compare against.
   * @param tolerance the tolerance allowed in which the two measures are defined
   *                  as near each
   *                  other.
   * @return true if this unit is near the other measure, otherwise false.
   */
  public boolean isNear(Time other, Time tolerance) {
    return Math.abs(this.baseUnitMagnitude() - other.baseUnitMagnitude()) <= Math
        .abs(tolerance.baseUnitMagnitude());
  }

  /** {@inheritDoc} */
  @Override
  public int compareTo(Time o) {
    return Double.compare(this.baseUnitMagnitude(), o.baseUnitMagnitude());
  }

  /**
   * Checks if this measure is greater than another measure of the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has a greater equivalent magnitude, false
   *         otherwise
   */
  public boolean gt(Time o) {
    return compareTo(o) > 0;
  }

  /**
   * Checks if this measure is greater than or equal to another measure of
   * the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has an equal or greater magnitude,
   *         false otherwise
   */
  public boolean gte(Time o) {
    return compareTo(o) > 0 || equals(o);
  }

  /**
   * Checks if this measure is less than another measure of the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has a lesser equivalent magnitude, false
   *         otherwise
   */
  public boolean lt(Time o) {
    return compareTo(o) < 0;
  }

  /**
   * Checks if this measure is less than or equal to another measure of
   * the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has an equal or lesser equivalent magnitude,
   *         false otherwise
   */
  public boolean lte(Time o) {
    return compareTo(o) < 0 || equals(o);
  }

  /**
   * Returns the measure with the absolute value closest to positive infinity.
   *
   * @param <U>      the type of the units of the measures
   * @param measures the set of measures to compare
   * @return the measure with the greatest positive magnitude, or null if no
   *         measures were provided
   */
  static Time max(Time... measures) {
    if (measures.length == 0) {
      return null; // nothing to compare
    }

    Time max = null;
    for (Time measure : measures) {
      if (max == null || measure.gt(max)) {
        max = measure;
      }
    }

    return max;
  }

  /**
   * Returns the measure with the absolute value closest to negative infinity.
   *
   * @param <U>      the type of the units of the measures
   * @param measures the set of measures to compare
   * @return the measure with the greatest negative magnitude
   */
  static Time min(Time... measures) {
    if (measures.length == 0) {
      return null; // nothing to compare
    }

    Time max = null;
    for (Time measure : measures) {
      if (max == null || measure.lt(max)) {
        max = measure;
      }
    }

    return max;
  }

  /**
   * Returns a string representation of this measurement in a shorthand form. The
   * symbol of the
   * backing unit is used, rather than the full name, and the magnitude is
   * represented in scientific
   * notation.
   *
   * @return the short form representation of this measurement
   */
  public String toShortString() {
    // eg 1.234e+04 V/m (1234 Volt per Meter in long form)
    return String.format("%.3e %s", magnitude(), unit().symbol());
  }

  /**
   * Returns a string representation of this measurement in a longhand form. The
   * name of the backing
   * unit is used, rather than its symbol, and the magnitude is represented in a
   * full string, not
   * scientific notation. (Very large values may be represented in scientific
   * notation, however)
   *
   * @return the long form representation of this measurement
   */
  public String toLongString() {
    // eg 1234 Volt per Meter (1.234e+04 V/m in short form)
    return String.format("%s %s", magnitude(), unit().name());
  }

  /**
   * Creates a new measure in the given unit with a magnitude equal to the given
   * one in base units.
   *
   * @param <U>               the type of the units of measure
   * @param baseUnitMagnitude the magnitude of the measure, in terms of the base
   *                          unit of measure
   * @param unit              the unit of measure
   * @return a new measure
   */
  public static Time ofBaseUnits(
      double baseUnitMagnitude, TimeUnit unit) {
    return new Time(unit.fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, unit);
  }

  /**
   * Creates a new measure in the given unit with a magnitude in terms of that
   * unit.
   *
   * @param <U>               the type of the units of measure
   * @param relativeMagnitude the magnitude of the measure
   * @param unit              the unit of measure
   * @return a new measure
   */
  public static Time ofRelativeUnits(
      double relativeMagnitude, TimeUnit unit) {
    return new Time(relativeMagnitude, unit.toBaseUnits(relativeMagnitude), unit);
  }

  /**
   * Checks for <i>object equality</i>. To check if two measures are
   * <i>equivalent</i>, use {@link
   * #isEquivalent(Measure) isEquivalent}.
   */
  @Override
  public boolean equals(Object o) {
    return o instanceof Time other
        && Objects.equals(m_unit, other.unit())
        && Math.abs(baseUnitMagnitude() - other.baseUnitMagnitude()) <= EQUIVALENCE_THRESHOLD;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_magnitude, m_unit);
  }

  @Override
  public String toString() {
    return toShortString();
  }

}
