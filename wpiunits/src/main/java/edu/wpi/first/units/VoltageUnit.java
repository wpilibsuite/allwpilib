// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Watts;

import edu.wpi.first.units.measure.ImmutableVoltage;
import edu.wpi.first.units.measure.MutVoltage;
import edu.wpi.first.units.measure.Voltage;

/**
 * Unit of electric voltage dimension.
 *
 * <p>This is the base type for units of voltage dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;VoltageUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Volts} and {@link Units#Millivolts}) can be found in the
 * {@link Units} class.
 */
public final class VoltageUnit extends PerUnit<PowerUnit, CurrentUnit> {
  private static final CombinatoryUnitCache<PowerUnit, CurrentUnit, VoltageUnit> cache =
      new CombinatoryUnitCache<>(VoltageUnit::new);

  VoltageUnit(PowerUnit power, CurrentUnit current) {
    super(
        power.isBaseUnit() && current.isBaseUnit()
            ? null
            : combine(power.getBaseUnit(), current.getBaseUnit()),
        power,
        current);
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
   * Combines a power and a current unit to form a unit of voltage.
   *
   * @param power the unit of energy
   * @param current the unit of time
   * @return the combined unit of power
   */
  public static VoltageUnit combine(PowerUnit power, CurrentUnit current) {
    return cache.combine(power, current);
  }

  @Override
  public VoltageUnit getBaseUnit() {
    return (VoltageUnit) super.getBaseUnit();
  }

  @Override
  public Voltage of(double magnitude) {
    return new ImmutableVoltage(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Voltage ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableVoltage(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public Voltage zero() {
    return (Voltage) super.zero();
  }

  @Override
  public Voltage one() {
    return (Voltage) super.one();
  }

  @Override
  public MutVoltage mutable(double magnitude) {
    return new MutVoltage(magnitude, toBaseUnits(magnitude), this);
  }

  /**
   * Constructs a unit of power equivalent to this unit of voltage multiplied by another unit of
   * electrical current. For example, {@code Volts.times(Amps)} will return a unit of power
   * equivalent to one Watt; {@code Volts.times(Milliamps)} will return a unit of power equivalent
   * to a milliwatt, and so on.
   *
   * @param current the current unit to multiply by
   * @return the power unit
   */
  @Override
  public PowerUnit mult(CurrentUnit current) {
    double baseUnitEquivalent = this.toBaseUnits(1) * current.toBaseUnits(1);
    UnaryFunction toBaseConverter = x -> x * baseUnitEquivalent;
    UnaryFunction fromBaseConverter = x -> x / baseUnitEquivalent;
    PowerUnit powerUnit =
        new PowerUnit(
            Watts,
            toBaseConverter,
            fromBaseConverter,
            this.name() + "-" + current.name(),
            this.symbol() + "*" + current.symbol());
    return powerUnit;
  }

  @Override
  public VelocityUnit<VoltageUnit> per(TimeUnit period) {
    return VelocityUnit.combine(this, period);
  }

  /**
   * Combines this unit of voltage with a unit of current to form a unit of resistance.
   *
   * @param currentUnit the unit of current
   * @return the combined resistance unit
   */
  public ResistanceUnit per(CurrentUnit currentUnit) {
    return ResistanceUnit.combine(this, currentUnit);
  }

  /**
   * Creates a generic ratio unit of this voltage to a different unit type.
   *
   * @param other the other unit type
   * @param <U> the type of the other unit
   * @return the combined ratio type
   */
  public <U extends Unit> PerUnit<VoltageUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another voltage unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other voltage unit
   * @param otherUnit the other voltage unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, VoltageUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
