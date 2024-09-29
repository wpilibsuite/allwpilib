// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Joules;
import static edu.wpi.first.units.Units.Seconds;

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

  /**
   * Combines voltage and current into power.
   *
   * @param voltage the unit of voltage
   * @param current the unit of current
   * @return the combined unit of power
   */
  public static PowerUnit combine(VoltageUnit voltage, CurrentUnit current) {
    return combine(
        new EnergyUnit(
            Joules,
            voltage.toBaseUnits(1) * current.toBaseUnits(1),
            voltage.name() + "-" + current.name(),
            voltage.symbol() + "*" + current.symbol()),
        Seconds);
  }

  /**
   * Combines voltage and current into power.
   *
   * @param current the unit of current
   * @param voltage the unit of voltage
   * @return the combined unit of power
   */
  public static PowerUnit combine(CurrentUnit current, VoltageUnit voltage) {
    return combine(voltage, current);
  }

  /**
   * Combines angular velocity and torque into power. Useful when dealing with motors and flywheels.
   *
   * @param angularVelocity the unit of angular velocity
   * @param torque the unit of torque
   * @return the combined unit of power
   */
  public static PowerUnit combine(AngularVelocityUnit angularVelocity, TorqueUnit torque) {
    return combine(
        new EnergyUnit(Joules, angularVelocity.toBaseUnits(1) * torque.toBaseUnits(1), "", ""),
        Seconds);
  }

  /**
   * Combines angular velocity and torque into power. Useful when dealing with motors and flywheels.
   *
   * @param torque the unit of torque
   * @param angularVelocity the unit of angular velocity
   * @return the combined unit of power
   */
  public static PowerUnit combine(TorqueUnit torque, AngularVelocityUnit angularVelocity) {
    return combine(angularVelocity, torque);
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
