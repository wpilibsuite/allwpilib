// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.dimensionless;

import java.util.Objects;
import java.util.Optional;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.UnaryFunction;

/**
 * Unit of measurement that defines an Dimensionless, such as radians,
 * rotations, or
 * degrees.
 *
 */
public class DimensionlessUnit {

  public static final DimensionlessUnit Value = new DimensionlessUnit();

  public static final DimensionlessUnit Percent = new DimensionlessUnit(1.0 / 100.0, "Percent", "%");

  private final UnaryFunction m_toBaseConverter;
  private final UnaryFunction m_fromBaseConverter;

  private final Optional<DimensionlessUnit> m_baseUnit;

  private Dimensionless m_zero;
  private Dimensionless m_one;

  private final String m_name;
  private final String m_symbol;

  /**
   * Creates a new Dimensionless unit defined by its relationship to the S.I. base
   * Dimensionless
   * unit (i.e. Value).
   *
   * @param toBaseConverter   a function for converting units of this type to
   *                          Radians.
   * @param fromBaseConverter a function for converting units of the other unit to
   *                          Radians.
   * @param name              the name of the Dimensionless unit. This should be a
   *                          singular noun
   *                          (so "Degree", not "Degrees")
   * @param symbol            the short symbol for the unit, such as "rad" for
   *                          Radians or "rev" for Revolutions.
   */
  public DimensionlessUnit(
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    m_baseUnit = Optional.of(Value);
    m_toBaseConverter = Objects.requireNonNull(toBaseConverter);
    m_fromBaseConverter = Objects.requireNonNull(fromBaseConverter);
    m_name = Objects.requireNonNull(name);
    m_symbol = Objects.requireNonNull(symbol);
  }

  /**
   * Creates a new unit with the given name and multiplier to the S.I. base
   * Dimensionless
   * unit (i.e. Radians).
   *
   * @param baseUnitEquivalent the multiplier to convert this unit to the other
   *                           unit of this type.
   *                           For example, meters has a multiplier of 1, mm has a
   *                           multiplier of 1e3, and km has
   *                           multiplier of 1e-3.
   * @param name               the name of the Dimensionless unit. This should be
   *                           a
   *                           singular noun
   *                           (so "Degree", not "Degrees")
   * @param symbol             the short symbol for the unit, such as "rad" for
   *                           Radians or "rev" for Revolutions.
   */
  public DimensionlessUnit(
      double baseUnitEquivalent,
      String name,
      String symbol) {
    this(
        x -> x * baseUnitEquivalent,
        x -> x / baseUnitEquivalent,
        name,
        symbol);
  }

  /**
   * Creates a new unit that is identical to other, but with a different name and
   * symbol.
   *
   * @param other  the other Dimensionless that this one is identical to.
   * @param name   the name of the Dimensionless unit. This should be a
   *               singular noun
   *               (so "Degree", not "Degrees")
   * @param symbol the short symbol for the unit, such as "rad" for
   *               Radians or "rev" for Revolutions.
   */
  public DimensionlessUnit(
      DimensionlessUnit other,
      String name,
      String symbol) {
    this(
        other.m_toBaseConverter,
        other.m_fromBaseConverter,
        name,
        symbol);
  }

  /**
   * Instantiates the base unit for Dimensionlesss (i.e. Radians). Only used once
   * by the
   * static field Radians.
   */
  private DimensionlessUnit() {
    m_baseUnit = Optional.empty();
    m_fromBaseConverter = x -> x;
    m_toBaseConverter = x -> x;
    m_name = "<>";
    m_symbol = "<>";
  }

  /**
   * Gets the base unit of measurement that this unit is derived from. If the unit
   * is the base unit,
   * the unit will be returned.
   *
   * <pre>
   * <code>
   *   Unit baseUnit = new Unit(null, ...);
   *   baseUnit.getBaseUnit(); // returns baseUnit
   *
   *   Unit derivedUnit = new Unit(baseUnit, ...);
   *   derivedUnit.getBaseUnit(); // returns baseUnit
   * </code>
   * </pre>
   *
   * @return the base unit
   */
  public DimensionlessUnit getBaseUnit() {
    return m_baseUnit.orElse(Value);
  }

  /**
   * Checks if this unit is the base unit for its own system of measurement.
   *
   * @return true if this is the base unit, false if not
   */
  public boolean isBaseUnit() {
    return this.equals(getBaseUnit());
  }

  /**
   * Checks if this unit is equivalent to another one. Equivalence is determined
   * by both units
   * having the same base type and treat the same base unit magnitude as the same
   * magnitude in their
   * own units, to within {@link Measure#EQUIVALENCE_THRESHOLD}.
   *
   * @param other the unit to compare to.
   * @return true if both units are equivalent, false if not
   */
  public boolean equivalent(DimensionlessUnit other) {

    double arbitrary = 16_777.214; // 2^24 / 1e3

    return Math.abs(
        this.m_fromBaseConverter.apply(arbitrary)
            - other.m_fromBaseConverter.apply(arbitrary)) <= Dimensionless.EQUIVALENCE_THRESHOLD
        && Math.abs(
            this.m_toBaseConverter.apply(arbitrary)
                - other.m_toBaseConverter.apply(arbitrary)) <= Dimensionless.EQUIVALENCE_THRESHOLD;
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
   * Converts a magnitude in terms of another unit of the same dimension to a
   * magnitude in terms of
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
  public double convertFrom(double magnitude, DimensionlessUnit otherUnit) {
    if (this.equivalent(otherUnit.getBaseUnit())) {
      // same unit, don't bother converting
      return magnitude;
    }
    return this.fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  /**
   * Gets the conversion function used to convert values to base unit terms. This
   * generally
   * shouldn't need to be used directly; prefer {@link #toBaseUnits(double)}
   * instead.
   *
   * @return the conversion function
   */
  public UnaryFunction getConverterToBase() {
    return m_toBaseConverter;
  }

  /**
   * Gets the conversion function used to convert values to terms of this unit.
   * This generally
   * shouldn't need to be used directly; prefer {@link #fromBaseUnits(double)}
   * instead.
   *
   * @return the conversion function
   */
  public UnaryFunction getConverterFromBase() {
    return m_fromBaseConverter;
  }

  /**
   * Creates a new measure of this unit with the given value. The resulting
   * measure is
   * <i>immutable</i> and cannot have its value modified.
   *
   * @param magnitude the magnitude of the measure to create
   * @return the measure
   */
  public Dimensionless of(double magnitude) {
    if (magnitude == 0) {
      // reuse static object
      return zero();
    }
    if (magnitude == 1) {
      // reuse static object
      return one();
    }
    return Dimensionless.ofRelativeUnits(magnitude, this);
  }

  /**
   * Creates a new measure with a magnitude equal to the given base unit
   * magnitude, converted to be
   * in terms of this unit.
   *
   * @param baseUnitMagnitude the magnitude of the measure in terms of the base
   *                          unit
   * @return the measure
   */
  public Dimensionless ofBaseUnits(double baseUnitMagnitude) {
    return Dimensionless.ofBaseUnits(baseUnitMagnitude, this);
  }

  /**
   * Gets a measure with a magnitude of 0 in terms of this unit.
   *
   * @return the zero-valued measure
   */
  public Dimensionless zero() {
    // lazy init because 'this' is null in object initialization
    if (m_zero == null) {
      m_zero = Dimensionless.ofRelativeUnits(0, this);
    }
    return m_zero;
  }

  /**
   * Gets a measure with a magnitude of 1 in terms of this unit.
   *
   * @return the 1-valued measure
   */
  public Dimensionless one() {
    // lazy init because 'this' is null in object initialization
    if (m_one == null) {
      m_one = Dimensionless.ofRelativeUnits(1, this);
    }
    return m_one;
  }

  @Override
  public boolean equals(Object o) {
    return this == o
        || o instanceof DimensionlessUnit that
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
