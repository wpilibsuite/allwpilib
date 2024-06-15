// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of energy dimension.
 *
 * <p>This is the base type for units of energy dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;EnergyUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Joules} and {@link Units#Kilojoules}) can be found in the
 * {@link Units} class.
 */
public class EnergyUnit extends Unit {
  EnergyUnit(
      EnergyUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  EnergyUnit(EnergyUnit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  @Override
  public EnergyUnit getBaseUnit() {
    return (EnergyUnit) super.getBaseUnit();
  }

  public PowerUnit per(TimeUnit period) {
    return PowerUnit.combine(this, period);
  }

  public Energy of(double magnitude) {
    return new Energy(magnitude, toBaseUnits(magnitude), this);
  }
}
