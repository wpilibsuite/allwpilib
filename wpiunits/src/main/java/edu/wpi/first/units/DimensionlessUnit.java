// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.Dimensionless;
import edu.wpi.first.units.measure.ImmutableDimensionless;
import edu.wpi.first.units.measure.MutDimensionless;

/**
 * A type of unit that corresponds to raw values and not any physical dimension, such as percentage.
 */
public final class DimensionlessUnit extends Unit {
  /**
   * Creates a new unit with the given name and multiplier to the base unit.
   *
   * @param baseUnitEquivalent the multiplier to convert this unit to the base unit of this type.
   * @param name the name of the unit
   * @param symbol the symbol of the unit
   */
  DimensionlessUnit(
      DimensionlessUnit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  DimensionlessUnit(
      DimensionlessUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  @Override
  public DimensionlessUnit getBaseUnit() {
    return (DimensionlessUnit) super.getBaseUnit();
  }

  @Override
  public Dimensionless of(double magnitude) {
    return new ImmutableDimensionless(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Dimensionless ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableDimensionless(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public Dimensionless zero() {
    return (Dimensionless) super.zero();
  }

  @Override
  public Dimensionless one() {
    return (Dimensionless) super.one();
  }

  @Override
  public MutDimensionless mutable(double initialMagnitude) {
    return new MutDimensionless(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  /**
   * Converts a measurement value in terms of another dimensionless unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other dimensionless unit
   * @param otherUnit the other dimensionless unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, DimensionlessUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  /**
   * Creates a frequency unit as the ratio of this dimensionless unit to the period of time in which
   * a single cycle is made.
   *
   * @param period the cycle period
   * @return the combined frequency unit
   */
  @Override
  public FrequencyUnit per(TimeUnit period) {
    return FrequencyUnit.combine(this, period);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<DimensionlessUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }
}
