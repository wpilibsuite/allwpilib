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
 */
public abstract class Unit {
  private final UnaryFunction m_toBaseConverter;
  private final UnaryFunction m_fromBaseConverter;

  private final Unit m_baseUnit;

  private final String m_name;
  private final String m_symbol;

  private final Measure<?> m_zero;
  private final Measure<?> m_one;

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
  @SuppressWarnings("this-escape")
  protected Unit(
      Unit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    m_baseUnit = baseUnit == null ? this : baseUnit;
    m_toBaseConverter = Objects.requireNonNull(toBaseConverter);
    m_fromBaseConverter = Objects.requireNonNull(fromBaseConverter);
    m_name = Objects.requireNonNull(name);
    m_symbol = Objects.requireNonNull(symbol);

    m_zero = of(0);
    m_one = of(1);
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
  protected Unit(Unit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    this(baseUnit, x -> x * baseUnitEquivalent, x -> x / baseUnitEquivalent, name, symbol);
  }

  /**
   * Creates a new immutable measurement of the given magnitude in terms of this unit.
   * Implementations are <strong>strongly</strong> recommended to sharpen the return type to a
   * unit-specific measurement implementation.
   *
   * @param magnitude the magnitude of the measurement.
   * @return the measurement object
   */
  public abstract Measure<?> of(double magnitude);

  /**
   * Creates a new immutable measurement of the given magnitude in terms of this unit's base unit.
   * Implementations are <strong>strongly</strong> recommended to sharpen the return type to a
   * unit-specific measurement implementation.
   *
   * @param baseUnitMagnitude the magnitude in terms of the base unit
   * @return the measurement object
   */
  public abstract Measure<?> ofBaseUnits(double baseUnitMagnitude);

  /**
   * Creates a new mutable measurement that is initialized to the given magnitude in terms of this
   * unit. Implementations are <strong>strongly</strong> recommended to sharpen the return type to a
   * unit-specific measurement implementation.
   *
   * @param initialMagnitude the initial magnitude of the mutable measurement
   * @return the mutable measurement object
   */
  public abstract MutableMeasure<?, ?, ?> mutable(double initialMagnitude);

  /**
   * Gets a measure of zero magnitude in terms of this unit. The returned object is guaranteed to be
   * of the same type returned by {@link #of(double)}. Subclasses are encouraged to override this
   * method to sharpen the return type.
   *
   * @return a zero-magnitude measure of this unit
   */
  public Measure<?> zero() {
    return m_zero;
  }

  /**
   * Gets a measure with a magnitude of 1.0 in terms of this unit. The returned object is guaranteed
   * to be of the same type returned by {@link #of(double)}. Subclasses are encouraged to override
   * this method to sharpen the return type.
   *
   * @return a measure of magnitude 1.0 in terms of this unit
   */
  public Measure<?> one() {
    return m_one;
  }

  /**
   * Combines this unit with a unit of time. This often - but not always - results in a velocity.
   * Subclasses should sharpen the return type to be unit-specific.
   *
   * @param time the unit of time
   * @return the combined unit
   */
  public abstract Unit per(TimeUnit time);

  /**
   * Gets the base unit of measurement that this unit is derived from. If the unit is the base unit,
   * the unit will be returned.
   *
   * <p><strong>NOTE:</strong> Subclasses <strong>must</strong> override this method to provide the
   * correct return type. Failing to do say will make unit combinations that use it break at
   * runtime!
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
  public Unit getBaseUnit() {
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
   * Checks if this unit is equivalent to another one. Equivalence is determined by both units
   * having the same base type and treat the same base unit magnitude as the same magnitude in their
   * own units, to within {@link Measure#EQUIVALENCE_THRESHOLD}.
   *
   * @param other the unit to compare to.
   * @return true if both units are equivalent, false if not
   */
  public boolean equivalent(Unit other) {
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
    return this == o
        || o instanceof Unit that
            && m_name.equals(that.m_name)
            && m_symbol.equals(that.m_symbol)
            && this.equivalent(that);
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
