// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import java.util.Objects;

/**
 * Unit of measurement that defines a quantity, such as grams, meters, or seconds.
 *
 * <p>This is the base class for units. Actual units (such as {@link Units#Grams} and {@link
 * Units#Meters}) can be found in the {@link Units} class.
 *
 * @param <U> the self type, e.g. {@code class SomeUnit extends Unit<SomeUnit>}
 */
public class Unit<U extends Unit<U>> {
  private final UnaryFunction m_toBaseConverter;
  private final UnaryFunction m_fromBaseConverter;

  private final U m_baseUnit;

  private Measure<U> m_zero;
  private Measure<U> m_one;

  private final String m_name;
  private final String m_symbol;

  /**
   * Creates a new unit defined by its relationship to some base unit.
   *
   * @param baseUnit the base unit, e.g. Meters for distances. Set this to {@code null} if the unit
   *     being constructed is its own base unit
   * @param toBaseConverter a function for converting units of this type to the base unit
   * @param fromBaseConverter a function for converting units of the base unit to this one
   * @param name the name of the unit. This should be a singular noun (so "Meter", not "Meters")
   * @param symbol the short symbol for the unit, such as "m" for meters or "lb." for pounds
   */
  @SuppressWarnings("unchecked")
  protected Unit(
      U baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    m_baseUnit = baseUnit == null ? (U) this : baseUnit;
    m_toBaseConverter = Objects.requireNonNull(toBaseConverter);
    m_fromBaseConverter = Objects.requireNonNull(fromBaseConverter);
    m_name = Objects.requireNonNull(name);
    m_symbol = Objects.requireNonNull(symbol);
  }

  /**
   * Creates a new unit with the given name and multiplier to the base unit.
   *
   * @param baseUnit the base unit, e.g. Meters for distances
   * @param baseUnitEquivalent the multiplier to convert this unit to the base unit of this type.
   *     For example, meters has a multiplier of 1, mm has a multiplier of 1e3, and km has
   *     multiplier of 1e-3.
   * @param name the name of the unit. This should be a singular noun (so "Meter", not "Meters")
   * @param symbol the short symbol for the unit, such as "m" for meters or "lb." for pounds
   */
  protected Unit(U baseUnit, double baseUnitEquivalent, String name, String symbol) {
    this(baseUnit, x -> x * baseUnitEquivalent, x -> x / baseUnitEquivalent, name, symbol);
  }

  /**
   * Gets the base unit of measurement that this unit is derived from. If the unit is the base unit,
   * the unit will be returned.
   *
   * <pre><code>
   *   Unit baseUnit = new Unit(null, ...);
   *   baseUnit.getBaseUnit(); // returns baseUnit
   *
   *   Unit derivedUnit = new Unit(baseUnit, ...);
   *   derivedUnit.getBaseUnit(); // returns baseUnit
   * </code></pre>
   *
   * @return the base unit
   */
  public U getBaseUnit() {
    return m_baseUnit;
  }

  /**
   * Checks if this unit is the base unit for its own system of measurement.
   *
   * @return true if this is the base unit, false if not
   */
  public boolean isBaseUnit() {
    return this.equals(m_baseUnit);
  }

  /**
   * Converts a value in terms of base units to a value in terms of this unit.
   *
   * @param valueInBaseUnits the value in base units to convert
   * @return the equivalent value in terms of this unit
   */
  public double fromBaseUnits(double valueInBaseUnits) {
    return m_fromBaseConverter.apply(valueInBaseUnits);
  }

  /**
   * Converts a value in terms of this unit to a value in terms of the base unit.
   *
   * @param valueInNativeUnits the value in terms of this unit to convert
   * @return the equivalent value in terms of the base unit
   */
  public double toBaseUnits(double valueInNativeUnits) {
    return m_toBaseConverter.apply(valueInNativeUnits);
  }

  /**
   * Converts a magnitude in terms of another unit of the same dimension to a magnitude in terms of
   * this unit.
   *
   * <pre>
   *   Inches.convertFrom(12, Feet) // 144.0
   *   Kilograms.convertFrom(2.2, Pounds) // 0.9979024
   * </pre>
   *
   * @param magnitude a magnitude measured in another unit
   * @param otherUnit the unit to convert the magnitude to
   * @return the corresponding value in terms of this unit.
   */
  public double convertFrom(double magnitude, Unit<U> otherUnit) {
    if (this.equivalent(otherUnit)) {
      // same unit, don't bother converting
      return magnitude;
    }
    return this.fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  /**
   * Gets the conversion function used to convert values to base unit terms. This generally
   * shouldn't need to be used directly; prefer {@link #toBaseUnits(double)} instead.
   *
   * @return the conversion function
   */
  public UnaryFunction getConverterToBase() {
    return m_toBaseConverter;
  }

  /**
   * Gets the conversion function used to convert values to terms of this unit. This generally
   * shouldn't need to be used directly; prefer {@link #fromBaseUnits(double)} instead.
   *
   * @return the conversion function
   */
  public UnaryFunction getConverterFromBase() {
    return m_fromBaseConverter;
  }

  /**
   * Creates a new measure of this unit with the given value. The resulting measure is
   * <i>immutable</i> and cannot have its value modified.
   *
   * @param magnitude the magnitude of the measure to create
   * @return the measure
   */
  public Measure<U> of(double magnitude) {
    if (magnitude == 0) {
      // reuse static object
      return zero();
    }
    if (magnitude == 1) {
      // reuse static object
      return one();
    }
    return ImmutableMeasure.ofRelativeUnits(magnitude, this);
  }

  /**
   * Creates a new measure with a magnitude equal to the given base unit magnitude, converted to be
   * in terms of this unit.
   *
   * @param baseUnitMagnitude the magnitude of the measure in terms of the base unit
   * @return the measure
   */
  public Measure<U> ofBaseUnits(double baseUnitMagnitude) {
    return ImmutableMeasure.ofBaseUnits(baseUnitMagnitude, this);
  }

  /**
   * Gets a measure with a magnitude of 0 in terms of this unit.
   *
   * @return the zero-valued measure
   */
  public Measure<U> zero() {
    // lazy init because 'this' is null in object initialization
    if (m_zero == null) {
      m_zero = ImmutableMeasure.ofRelativeUnits(0, this);
    }
    return m_zero;
  }

  /**
   * Gets a measure with a magnitude of 1 in terms of this unit.
   *
   * @return the 1-valued measure
   */
  public Measure<U> one() {
    // lazy init because 'this' is null in object initialization
    if (m_one == null) {
      m_one = ImmutableMeasure.ofRelativeUnits(1, this);
    }
    return m_one;
  }

  /**
   * Creates a velocity unit derived from this one. Can be chained to denote velocity, acceleration,
   * jerk, etc.
   *
   * <pre>
   *   Meters.per(Second) // linear velocity
   *   Kilograms.per(Second) // mass flow
   *   Feet.per(Second).per(Second).of(32) // roughly 1G of acceleration
   * </pre>
   *
   * @param period the time period of the velocity, such as seconds or milliseconds
   * @return a velocity unit corresponding to the rate of change of this unit over time
   */
  public Velocity<U> per(Time period) {
    return Velocity.combine(this, period);
  }

  /**
   * Takes this unit and creates a new proportional unit where this unit is the numerator and the
   * given denominator is the denominator.
   *
   * <pre>
   *   Volts.per(Meter) // V/m
   * </pre>
   *
   * @param <D> the type of the denominator units
   * @param denominator the denominator of the proportional unit
   * @return a combined proportional unit
   */
  @SuppressWarnings("unchecked")
  public <D extends Unit<D>> Per<U, D> per(D denominator) {
    return Per.combine((U) this, denominator);
  }

  /**
   * Takes this unit and creates a new combinatory unit equivalent to this unit multiplied by
   * another.
   *
   * <pre>
   *   Volts.mult(Meter) // V*m
   * </pre>
   *
   * @param <U2> the type of the unit to multiply by
   * @param other the unit to multiply by
   * @return a combined unit equivalent to this unit multiplied by the other
   */
  @SuppressWarnings("unchecked")
  public <U2 extends Unit<U2>> Mult<U, U2> mult(U2 other) {
    return Mult.combine((U) this, other);
  }

  /**
   * Checks if this unit is equivalent to another one. Equivalence is determined by both units
   * having the same base type and treat the same base unit magnitude as the same magnitude in their
   * own units, to within {@link Measure#EQUIVALENCE_THRESHOLD}.
   *
   * @param other the unit to compare to.
   * @return true if both units are equivalent, false if not
   */
  public boolean equivalent(Unit<?> other) {
    if (!getClass().equals(other.getClass())) {
      // different unit types, not compatible
      return false;
    }

    double arbitrary = 16_777.214; // 2^24 / 1e3

    return Math.abs(
                this.m_fromBaseConverter.apply(arbitrary)
                    - other.m_fromBaseConverter.apply(arbitrary))
            <= Measure.EQUIVALENCE_THRESHOLD
        && Math.abs(
                this.m_toBaseConverter.apply(arbitrary) - other.m_toBaseConverter.apply(arbitrary))
            <= Measure.EQUIVALENCE_THRESHOLD;
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (!(o instanceof Unit)) {
      return false;
    }
    Unit<?> that = (Unit<?>) o;
    return m_name.equals(that.m_name) && m_symbol.equals(that.m_symbol) && this.equivalent(that);
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_toBaseConverter, m_fromBaseConverter, m_name, m_symbol);
  }

  /**
   * Gets the name of this unit.
   *
   * @return the unit's name
   */
  public String name() {
    return m_name;
  }

  /**
   * Gets the symbol of this unit.
   *
   * @return the unit's symbol
   */
  public String symbol() {
    return m_symbol;
  }

  @Override
  public String toString() {
    return name();
  }
}
