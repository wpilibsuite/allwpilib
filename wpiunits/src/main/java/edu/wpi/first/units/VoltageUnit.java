// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of electric voltage dimension.
 *
 * <p>This is the base type for units of voltage dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;VoltageUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Volts} and {@link Units#Millivolts}) can be found in the
 * {@link Units} class.
 */
public class VoltageUnit extends Unit {
  VoltageUnit(VoltageUnit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  VoltageUnit(
      VoltageUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Constructs a unit of power equivalent to this unit of voltage multiplied by another unit of
   * electrical current. For example, {@code Volts.times(Amps)} will return a unit of power
   * equivalent to one Watt; {@code Volts.times(Milliamps)} will return a unit of power equivalent
   * to a milliwatt, and so on.
   *
   * @param current the current unit to multiply by
   * @param name the name of the resulting unit of power
   * @param symbol the symbol used to represent the unit of power
   * @return the power unit
   */
  public PowerUnit times(CurrentUnit current, String name, String symbol) {
    return Units.derive(PowerUnit.combine(this, current)).named(name).symbol(symbol).make();
  }

  public Voltage of(double magnitude) {
    return new Voltage(magnitude, toBaseUnits(magnitude), this);
  }

  public Voltage.Mutable mutable(double magnitude) {
    return new Voltage.Mutable(magnitude, toBaseUnits(magnitude), this);
  }

  public <U extends Unit> Per<VoltageUnit, U> per(U other) {
    return Per.combine(this, other);
  }

  public VelocityUnit<VoltageUnit> per(TimeUnit period) {
    return VelocityUnit.combine(this, period);
  }

  public double convertFrom(double magnitude, VoltageUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
