// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableVoltagePerDistancePerTimeSquared;
import edu.wpi.first.units.measure.MutVoltagePerDistancePerTimeSquared;
import edu.wpi.first.units.measure.VoltagePerDistancePerTimeSquared;

/**
 * A unit for measuring linear mechanisms' feedforward voltages based on a model of the system and a
 * desired commaned linear acceleration like {@link Units#VoltsPerMeterPerSecondSquared}.
 */
public final class VoltagePerDistancePerTimeSquaredUnit
    extends PerUnit<VoltageUnit, LinearAccelerationUnit> {
  private static final CombinatoryUnitCache<
          VoltageUnit, LinearAccelerationUnit, VoltagePerDistancePerTimeSquaredUnit>
      cache = new CombinatoryUnitCache<>(VoltagePerDistancePerTimeSquaredUnit::new);

  VoltagePerDistancePerTimeSquaredUnit(VoltageUnit numerator, LinearAccelerationUnit denominator) {
    super(
        numerator.isBaseUnit() && denominator.isBaseUnit()
            ? null
            : combine(numerator.getBaseUnit(), denominator.getBaseUnit()),
        numerator,
        denominator);
  }

  VoltagePerDistancePerTimeSquaredUnit(
      VoltagePerDistancePerTimeSquaredUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a voltage unit and a linear acceleration unit into a unit of voltage per distance per
   * time squared.
   *
   * @param voltage the unit of voltage
   * @param linearAcceleration the unit of linear acceleration
   * @return the combined voltage per distance per time unit
   */
  public static VoltagePerDistancePerTimeSquaredUnit combine(
      VoltageUnit voltage, LinearAccelerationUnit linearAcceleration) {
    return cache.combine(voltage, linearAcceleration);
  }

  @Override
  public VoltagePerDistancePerTimeSquaredUnit getBaseUnit() {
    return (VoltagePerDistancePerTimeSquaredUnit) super.getBaseUnit();
  }

  @Override
  public VoltagePerDistancePerTimeSquared of(double magnitude) {
    return new ImmutableVoltagePerDistancePerTimeSquared(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public VoltagePerDistancePerTimeSquared ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableVoltagePerDistancePerTimeSquared(
        fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public VoltagePerDistancePerTimeSquared zero() {
    return (VoltagePerDistancePerTimeSquared) super.zero();
  }

  @Override
  public VoltagePerDistancePerTimeSquared one() {
    return (VoltagePerDistancePerTimeSquared) super.one();
  }

  @Override
  public MutVoltagePerDistancePerTimeSquared mutable(double initialMagnitude) {
    return new MutVoltagePerDistancePerTimeSquared(
        initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<VoltagePerDistancePerTimeSquaredUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, VoltagePerDistancePerTimeSquaredUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
