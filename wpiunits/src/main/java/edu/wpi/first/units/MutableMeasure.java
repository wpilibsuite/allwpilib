// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import java.util.Objects;

/**
 * A specialization of {@link Measure} that allows for mutability. This is intended to be used for
 * memory use reasons (such as on the memory-restricted roboRIO 1 or 2 or SBC coprocessors) and
 * should NOT be exposed in the public API for a class that uses it.
 *
 * <p>The advantage of using this class is to reuse one instance of a measurement object, as opposed
 * to instantiating a new immutable instance every time an operation is performed. This will reduce
 * memory pressure, but comes at the cost of increased code complexity and sensitivity to race
 * conditions if misused.
 *
 * <p>Any unsafe methods are prefixed with {@code mut_*}, such as {@link #mut_plus(Measure)} or
 * {@link #mut_replace(Measure)}. These methods will change the internal state of the measurement
 * object, and as such can be dangerous to use. They are primarily intended for use to track
 * internal state of things like sensors
 *
 * @param <U> the type of the unit of measure
 */
public final class MutableMeasure<U extends Unit<U>> implements Measure<U> {
  private double m_magnitude;
  private double m_baseUnitMagnitude;
  private U m_unit;

  private MutableMeasure(double initialMagnitude, double baseUnitMagnitude, U unit) {
    m_magnitude = initialMagnitude;
    m_baseUnitMagnitude = baseUnitMagnitude;
    m_unit = unit;
  }

  /**
   * Creates a new mutable measure that is a copy of the given one.
   *
   * @param <U> the type of the units of measure
   * @param measure the measure to create a mutable copy of
   * @return a new mutable measure with an initial state equal to the given measure
   */
  public static <U extends Unit<U>> MutableMeasure<U> mutable(Measure<U> measure) {
    return new MutableMeasure<>(measure.magnitude(), measure.baseUnitMagnitude(), measure.unit());
  }

  /**
   * Creates a new mutable measure with a magnitude of 0 in the given unit.
   *
   * @param <U> the type of the units of measure
   * @param unit the unit of measure
   * @return a new mutable measure
   */
  public static <U extends Unit<U>> MutableMeasure<U> zero(U unit) {
    return mutable(unit.zero());
  }

  /**
   * Creates a new mutable measure in the given unit with a magnitude equal to the given one in base
   * units.
   *
   * @param <U> the type of the units of measure
   * @param baseUnitMagnitude the magnitude of the measure, in terms of the base unit of measure
   * @param unit the unit of measure
   * @return a new mutable measure
   */
  public static <U extends Unit<U>> MutableMeasure<U> ofBaseUnits(
      double baseUnitMagnitude, U unit) {
    return new MutableMeasure<>(unit.fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, unit);
  }

  /**
   * Creates a new mutable measure in the given unit with a magnitude in terms of that unit.
   *
   * @param <U> the type of the units of measure
   * @param relativeMagnitude the magnitude of the measure
   * @param unit the unit of measure
   * @return a new mutable measure
   */
  public static <U extends Unit<U>> MutableMeasure<U> ofRelativeUnits(
      double relativeMagnitude, U unit) {
    return new MutableMeasure<>(relativeMagnitude, unit.toBaseUnits(relativeMagnitude), unit);
  }

  @Override
  public double magnitude() {
    return m_magnitude;
  }

  @Override
  public double baseUnitMagnitude() {
    return m_baseUnitMagnitude;
  }

  @Override
  public U unit() {
    return m_unit;
  }

  // UNSAFE

  /**
   * Sets the new magnitude of the measurement. The magnitude must be in terms of the {@link
   * #unit()}.
   *
   * @param magnitude the new magnitude of the measurement
   */
  public void mut_setMagnitude(double magnitude) {
    m_magnitude = magnitude;
    m_baseUnitMagnitude = m_unit.toBaseUnits(magnitude);
  }

  /**
   * Sets the new magnitude of the measurement. The magnitude must be in terms of the base unit of
   * the current unit.
   *
   * @param baseUnitMagnitude the new magnitude of the measurement
   */
  public void mut_setBaseUnitMagnitude(double baseUnitMagnitude) {
    m_baseUnitMagnitude = baseUnitMagnitude;
    m_magnitude = m_unit.fromBaseUnits(baseUnitMagnitude);
  }

  /**
   * Overwrites the state of this measure and replaces it with values from the given one.
   *
   * @param other the other measure to copy values from
   * @return this measure
   */
  public MutableMeasure<U> mut_replace(Measure<U> other) {
    m_magnitude = other.magnitude();
    m_baseUnitMagnitude = other.baseUnitMagnitude();
    m_unit = other.unit();
    return this;
  }

  /**
   * Overwrites the state of this measure with new values.
   *
   * @param magnitude the new magnitude in terms of the new unit
   * @param unit the new unit
   * @return this measure
   */
  public MutableMeasure<U> mut_replace(double magnitude, U unit) {
    this.m_magnitude = magnitude;
    this.m_baseUnitMagnitude = unit.toBaseUnits(magnitude);
    this.m_unit = unit;
    return this;
  }

  /**
   * Increments the current magnitude of the measure by the given value. The value must be in terms
   * of the current {@link #unit() unit}.
   *
   * @param raw the raw value to accumulate by
   * @return the measure
   */
  public MutableMeasure<U> mut_acc(double raw) {
    this.m_magnitude += raw;
    this.m_baseUnitMagnitude += m_unit.toBaseUnits(raw);
    return this;
  }

  /**
   * Increments the current magnitude of the measure by the amount of the given measure.
   *
   * @param other the measure whose value should be added to this one
   * @return the measure
   */
  public MutableMeasure<U> mut_acc(Measure<U> other) {
    m_baseUnitMagnitude += other.baseUnitMagnitude();

    // can't naively use m_magnitude += other.in(m_unit) because the units may not
    // be scalar multiples (eg adding 0C to 100K should result in 373.15K, not 100K)
    m_magnitude = m_unit.fromBaseUnits(m_baseUnitMagnitude);
    return this;
  }

  // Math

  /**
   * Adds another measurement to this one. This will mutate the object instead of generating a new
   * measurement object.
   *
   * @param other the measurement to add
   * @return this measure
   */
  public MutableMeasure<U> mut_plus(Measure<U> other) {
    return mut_plus(other.magnitude(), other.unit());
  }

  /**
   * Adds another measurement to this one. This will mutate the object instead of generating a new
   * measurement object. This is a denormalized version of {@link #mut_plus(Measure)} to avoid
   * having to wrap raw numbers in a {@code Measure} object and pay for an object allocation.
   *
   * @param magnitude the magnitude of the other measurement.
   * @param unit the unit of the other measurement
   * @return this measure
   */
  public MutableMeasure<U> mut_plus(double magnitude, U unit) {
    mut_setBaseUnitMagnitude(m_baseUnitMagnitude + unit.toBaseUnits(magnitude));
    return this;
  }

  /**
   * Subtracts another measurement to this one. This will mutate the object instead of generating a
   * new measurement object.
   *
   * @param other the measurement to add
   * @return this measure
   */
  public MutableMeasure<U> mut_minus(Measure<U> other) {
    return mut_minus(other.magnitude(), other.unit());
  }

  /**
   * Subtracts another measurement to this one. This will mutate the object instead of generating a
   * new measurement object. This is a denormalized version of {@link #mut_minus(Measure)} to avoid
   * having to wrap raw numbers in a {@code Measure} object and pay for an object allocation.
   *
   * @param magnitude the magnitude of the other measurement.
   * @param unit the unit of the other measurement
   * @return this measure
   */
  public MutableMeasure<U> mut_minus(double magnitude, U unit) {
    return mut_plus(-magnitude, unit);
  }

  /**
   * Multiplies this measurement by some constant value. This will mutate the object instead of
   * generating a new measurement object.
   *
   * @param multiplier the multiplier to scale the measurement by
   * @return this measure
   */
  public MutableMeasure<U> mut_times(double multiplier) {
    mut_setBaseUnitMagnitude(m_baseUnitMagnitude * multiplier);
    return this;
  }

  /**
   * Multiplies this measurement by some constant value. This will mutate the object instead of
   * generating a new measurement object.
   *
   * @param multiplier the multiplier to scale the measurement by
   * @return this measure
   */
  public MutableMeasure<U> mut_times(Measure<? extends Dimensionless> multiplier) {
    return mut_times(multiplier.baseUnitMagnitude());
  }

  /**
   * Divides this measurement by some constant value. This will mutate the object instead of
   * generating a new measurement object.
   *
   * @param divisor the divisor to scale the measurement by
   * @return this measure
   */
  public MutableMeasure<U> mut_divide(double divisor) {
    mut_setBaseUnitMagnitude(m_baseUnitMagnitude / divisor);
    return this;
  }

  /**
   * Divides this measurement by some constant value. This will mutate the object instead of
   * generating a new measurement object.
   *
   * @param divisor the divisor to scale the measurement by
   * @return this measure
   */
  public MutableMeasure<U> mut_divide(Measure<? extends Dimensionless> divisor) {
    return mut_divide(divisor.baseUnitMagnitude());
  }

  @Override
  public Measure<U> copy() {
    return new ImmutableMeasure<>(m_magnitude, m_baseUnitMagnitude, m_unit);
  }

  @Override
  public String toString() {
    return toShortString();
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (!(o instanceof Measure)) {
      return false;
    }
    Measure<?> that = (Measure<?>) o;
    return Objects.equals(m_unit, that.unit()) && this.isEquivalent(that);
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_magnitude, m_unit);
  }
}
