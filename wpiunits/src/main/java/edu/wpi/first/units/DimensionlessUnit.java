// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.immutable.ImmutableDimensionless;
import edu.wpi.first.units.measure.Dimensionless;
import edu.wpi.first.units.mutable.MutDimensionless;

/**
 * A type of unit that corresponds to raw values and not any physical dimension, such as percentage.
 */
public class DimensionlessUnit extends Unit {
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
  public Dimensionless of(double magnitude) {
    return new ImmutableDimensionless(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Dimensionless ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableDimensionless(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public MutDimensionless mutable(double initialMagnitude) {
    return new MutDimensionless(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  public <U extends Unit> U mult(U other) {
    return Units.derive(other)
        .toBase(other.getConverterToBase().mult(this.getConverterToBase()))
        .fromBase(other.getConverterFromBase().mult(this.getConverterFromBase()))
        // TODO: name, symbol
        .make();
  }

  public double convertFrom(double magnitude, DimensionlessUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  public FrequencyUnit per(TimeUnit period) {
    return FrequencyUnit.combine(this, period);
  }
}
