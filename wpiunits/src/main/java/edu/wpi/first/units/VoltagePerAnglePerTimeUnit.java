// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableVoltagePerAnglePerTime;
import edu.wpi.first.units.measure.MutVoltagePerAnglePerTime;
import edu.wpi.first.units.measure.VoltagePerAnglePerTime;

/**
 * A unit for measuring angular mechanisms' feedforward voltages based on a model of the system and
 * a desired commaned angular velocity like {@link Units#VoltsPerRadianPerSecond}.
 */
public final class VoltagePerAnglePerTimeUnit extends PerUnit<VoltageUnit, AngularVelocityUnit> {
  private static final CombinatoryUnitCache<
          VoltageUnit, AngularVelocityUnit, VoltagePerAnglePerTimeUnit>
      cache = new CombinatoryUnitCache<>(VoltagePerAnglePerTimeUnit::new);

  VoltagePerAnglePerTimeUnit(VoltageUnit numerator, AngularVelocityUnit denominator) {
    super(
        numerator.isBaseUnit() && denominator.isBaseUnit()
            ? null
            : combine(numerator.getBaseUnit(), denominator.getBaseUnit()),
        numerator,
        denominator);
  }

  VoltagePerAnglePerTimeUnit(
      VoltagePerAnglePerTimeUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a voltage unit and a angular velocity unit into a unit of voltage per angle per time.
   *
   * @param voltage the unit of voltage
   * @param angularVelocity the unit of angular velocity
   * @return the combined voltage per angle per time unit
   */
  public static VoltagePerAnglePerTimeUnit combine(
      VoltageUnit voltage, AngularVelocityUnit angularVelocity) {
    return cache.combine(voltage, angularVelocity);
  }

  @Override
  public VoltagePerAnglePerTimeUnit getBaseUnit() {
    return (VoltagePerAnglePerTimeUnit) super.getBaseUnit();
  }

  @Override
  public VoltagePerAnglePerTime of(double magnitude) {
    return new ImmutableVoltagePerAnglePerTime(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public VoltagePerAnglePerTime ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableVoltagePerAnglePerTime(
        fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public VoltagePerAnglePerTime zero() {
    return (VoltagePerAnglePerTime) super.zero();
  }

  @Override
  public VoltagePerAnglePerTime one() {
    return (VoltagePerAnglePerTime) super.one();
  }

  @Override
  public MutVoltagePerAnglePerTime mutable(double initialMagnitude) {
    return new MutVoltagePerAnglePerTime(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<VoltagePerAnglePerTimeUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, VoltagePerAnglePerTimeUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
