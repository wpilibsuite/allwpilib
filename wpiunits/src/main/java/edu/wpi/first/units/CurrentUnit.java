// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.ImmutableCurrent;
import edu.wpi.first.units.measure.MutCurrent;

/**
 * Unit of electric current dimension.
 *
 * <p>This is the base type for units of current dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;CurrentUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Amps} and {@link Units#Milliamps}) can be found in the
 * {@link Units} class.
 */
public final class CurrentUnit extends Unit {
  CurrentUnit(CurrentUnit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  CurrentUnit(
      CurrentUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  @Override
  public CurrentUnit getBaseUnit() {
    return (CurrentUnit) super.getBaseUnit();
  }

  /**
   * Constructs a unit of power equivalent to this unit of electrical current multiplied by another
   * unit of voltage. For example, {@code Amps.times(Volts)} will return a unit of power equivalent
   * to one Watt; {@code Amps.times(Millivolts)} will return a unit of power equivalent to a
   * milliwatt, and so on.
   *
   * @param voltage the voltage unit to multiply by
   * @param name the name of the resulting unit of power
   * @param symbol the symbol used to represent the unit of power
   * @return the power unit
   */
  public PowerUnit mult(VoltageUnit voltage, String name, String symbol) {
    return Units.derive(PowerUnit.combine(voltage, this)).named(name).symbol(symbol).make();
  }

  @Override
  public Current of(double magnitude) {
    return new ImmutableCurrent(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Current ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableCurrent(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public Current zero() {
    return (Current) super.zero();
  }

  @Override
  public Current one() {
    return (Current) super.one();
  }

  @Override
  public MutCurrent mutable(double initialMagnitude) {
    return new MutCurrent(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  @Override
  public VelocityUnit<CurrentUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<CurrentUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another current unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other current unit
   * @param otherUnit the other current unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, CurrentUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
