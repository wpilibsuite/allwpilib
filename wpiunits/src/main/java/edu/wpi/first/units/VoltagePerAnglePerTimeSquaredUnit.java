// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableVoltagePerAnglePerTimeSquared;
import edu.wpi.first.units.measure.MutVoltagePerAnglePerTimeSquared;
import edu.wpi.first.units.measure.VoltagePerAnglePerTimeSquared;

/**
 * A unit for measuring angular mechanisms' feedforward voltages based on a model of the system and
 * a desired commaned angular acceleration like {@link Units#VoltsPerRadianPerSecondSquared}.
 */
public final class VoltagePerAnglePerTimeSquaredUnit
    extends PerUnit<VoltageUnit, AngularAccelerationUnit> {
  private static final CombinatoryUnitCache<
          VoltageUnit, AngularAccelerationUnit, VoltagePerAnglePerTimeSquaredUnit>
      cache = new CombinatoryUnitCache<>(VoltagePerAnglePerTimeSquaredUnit::new);

  VoltagePerAnglePerTimeSquaredUnit(VoltageUnit numerator, AngularAccelerationUnit denominator) {
    super(
        numerator.isBaseUnit() && denominator.isBaseUnit()
            ? null
            : combine(numerator.getBaseUnit(), denominator.getBaseUnit()),
        numerator,
        denominator);
  }

  VoltagePerAnglePerTimeSquaredUnit(
      VoltagePerAnglePerTimeSquaredUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a voltage unit and a angular acceleration unit into a unit of voltage per angle per
   * time squared.
   *
   * @param voltage the unit of voltage
   * @param angularAcceleration the unit of angular acceleration
   * @return the combined voltage per angle per time unit
   */
  public static VoltagePerAnglePerTimeSquaredUnit combine(
      VoltageUnit voltage, AngularAccelerationUnit angularAcceleration) {
    return cache.combine(voltage, angularAcceleration);
  }

  @Override
  public VoltagePerAnglePerTimeSquaredUnit getBaseUnit() {
    return (VoltagePerAnglePerTimeSquaredUnit) super.getBaseUnit();
  }

  @Override
  public VoltagePerAnglePerTimeSquared of(double magnitude) {
    return new ImmutableVoltagePerAnglePerTimeSquared(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public VoltagePerAnglePerTimeSquared ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableVoltagePerAnglePerTimeSquared(
        fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public VoltagePerAnglePerTimeSquared zero() {
    return (VoltagePerAnglePerTimeSquared) super.zero();
  }

  @Override
  public VoltagePerAnglePerTimeSquared one() {
    return (VoltagePerAnglePerTimeSquared) super.one();
  }

  @Override
  public MutVoltagePerAnglePerTimeSquared mutable(double initialMagnitude) {
    return new MutVoltagePerAnglePerTimeSquared(
        initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<VoltagePerAnglePerTimeSquaredUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, VoltagePerAnglePerTimeSquaredUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
