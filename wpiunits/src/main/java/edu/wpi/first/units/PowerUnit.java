// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Amps;

import edu.wpi.first.units.measure.ImmutablePower;
import edu.wpi.first.units.measure.MutPower;
import edu.wpi.first.units.measure.Power;

/**
 * Unit of power dimension.
 *
 * <p>This is the base type for units of power dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;PowerUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Watts} and {@link Units#Horsepower}) can be found in the
 * {@link Units} class.
 */
public final class PowerUnit extends PerUnit<EnergyUnit, TimeUnit> {
  private static final CombinatoryUnitCache<EnergyUnit, TimeUnit, PowerUnit> cache =
      new CombinatoryUnitCache<>(PowerUnit::new);

  PowerUnit(EnergyUnit energy, TimeUnit time) {
    super(
        energy.isBaseUnit() && time.isBaseUnit()
            ? null
            : combine(energy.getBaseUnit(), time.getBaseUnit()),
        energy,
        time);
  }

  PowerUnit(
      PowerUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines an energy and a time unit to form a unit of power.
   *
   * @param energy the unit of energy
   * @param period the unit of time
   * @return the combined unit of power
   */
  public static PowerUnit combine(EnergyUnit energy, TimeUnit period) {
    return cache.combine(energy, period);
  }

  @Override
  public PowerUnit getBaseUnit() {
    return (PowerUnit) super.getBaseUnit();
  }

  @Override
  public Power of(double magnitude) {
    return new ImmutablePower(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Power ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutablePower(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public Power zero() {
    return (Power) super.zero();
  }

  @Override
  public Power one() {
    return (Power) super.one();
  }

  @Override
  public MutPower mutable(double initialMagnitude) {
    return new MutPower(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  /**
   * Constructs a unit of voltage equivalent to this unit of power divided by another unit of
   * current. For example, {@code Watts.per(Amps)} will return a unit of power equivalent to one
   * Volt.
   *
   * @param current the current unit to multiply by
   * @return the power unit
   */  
  public VoltageUnit per(CurrentUnit current) {
    return VoltageUnit.combine(this, current);
  }

  /**
   * Constructs a unit of current equivalent to this unit of power divided by another unit of
   * voltage. For example, {@code Watts.per(Volts)} will return a unit of power equivalent to one
   * Amp.
   *
   * @param voltage the voltage unit to multiply by
   * @return the power unit
   */
  public CurrentUnit per(VoltageUnit voltage) {
    double baseUnitEquivalent = this.toBaseUnits(1) / voltage.toBaseUnits(1);
    UnaryFunction toBaseConverter = x -> x * baseUnitEquivalent;
    UnaryFunction fromBaseConverter = x -> x / baseUnitEquivalent;
    CurrentUnit currentUnit =
        new CurrentUnit(
            Amps,
            toBaseConverter,
            fromBaseConverter,
            this.name() + " per " + voltage.name(),
            this.symbol() + "/" + voltage.symbol());
    return Units.derive(currentUnit).make();
  }

  @Override
  public VelocityUnit<PowerUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<PowerUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another power unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other power unit
   * @param otherUnit the other power unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, PowerUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
