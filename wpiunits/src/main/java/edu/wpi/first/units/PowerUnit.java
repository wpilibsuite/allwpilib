// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Joules;
import static edu.wpi.first.units.Units.Seconds;

/**
 * Unit of power dimension.
 *
 * <p>This is the base type for units of power dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;PowerUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Watts} and {@link Units#Horsepower}) can be found in the
 * {@link Units} class.
 */
public class PowerUnit extends PerUnit<EnergyUnit, TimeUnit> {
  private static final CombinatoryUnitCache<EnergyUnit, TimeUnit, PowerUnit> cache =
      new CombinatoryUnitCache<>(PowerUnit::new);

  protected PowerUnit(EnergyUnit energy, TimeUnit time) {
    this(
        energy.isBaseUnit() && time.isBaseUnit()
            ? null
            : combine(energy.getBaseUnit(), time.getBaseUnit()),
        (x) -> x * energy.toBaseUnits(1) / time.toBaseUnits(1),
        (x) -> x * time.toBaseUnits(1) / energy.toBaseUnits(1),
        energy.name() + " per " + time.name(),
        energy.symbol() + "/" + time.symbol());
  }

  PowerUnit(
      PowerUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public static PowerUnit combine(EnergyUnit energy, TimeUnit period) {
    return cache.combine(energy, period);
  }

  public static PowerUnit combine(VoltageUnit voltage, CurrentUnit current) {
    return combine(
        new EnergyUnit(
            Joules,
            voltage.toBaseUnits(1) * current.toBaseUnits(1),
            voltage.name() + "-" + current.name(),
            voltage.symbol() + "*" + current.symbol()),
        Seconds);
  }

  public static PowerUnit combine(CurrentUnit current, VoltageUnit voltage) {
    return combine(voltage, current);
  }

  public static PowerUnit combine(AngularVelocityUnit angularVelocity, TorqueUnit torque) {
    return combine(
        new EnergyUnit(Joules, angularVelocity.toBaseUnits(1) * torque.toBaseUnits(1), "", ""),
        Seconds);
  }

  public static PowerUnit combine(TorqueUnit torque, AngularVelocityUnit angularVelocity) {
    return combine(angularVelocity, torque);
  }

  @Override
  public Power of(double magnitude) {
    return new Power(magnitude, toBaseUnits(magnitude), this);
  }

  public double convertFrom(double magnitude, PowerUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
