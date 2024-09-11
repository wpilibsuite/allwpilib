// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableVoltagePerDistancePerTime;
import edu.wpi.first.units.measure.MutVoltagePerDistancePerTime;
import edu.wpi.first.units.measure.VoltagePerDistancePerTime;

/**
 * A unit for measuring linear mechanisms' feedforward voltages based on a model of the system and a
 * desired commaned linear velocity like {@link Units#VoltsPerMeterPerSecond}.
 */
public final class VoltagePerDistancePerTimeUnit extends PerUnit<VoltageUnit, LinearVelocityUnit> {
  private static final CombinatoryUnitCache<
          VoltageUnit, LinearVelocityUnit, VoltagePerDistancePerTimeUnit>
      cache = new CombinatoryUnitCache<>(VoltagePerDistancePerTimeUnit::new);

  VoltagePerDistancePerTimeUnit(VoltageUnit numerator, LinearVelocityUnit denominator) {
    super(
        numerator.isBaseUnit() && denominator.isBaseUnit()
            ? null
            : combine(numerator.getBaseUnit(), denominator.getBaseUnit()),
        numerator,
        denominator);
  }

  VoltagePerDistancePerTimeUnit(
      VoltagePerDistancePerTimeUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a voltage unit and a linear velocity unit into a unit of voltage per distance per
   * time.
   *
   * @param voltage the unit of voltage
   * @param linearVelocity the unit of linear velocity
   * @return the combined voltage per distance per time unit
   */
  public static VoltagePerDistancePerTimeUnit combine(
      VoltageUnit voltage, LinearVelocityUnit linearVelocity) {
    return cache.combine(voltage, linearVelocity);
  }

  @Override
  public VoltagePerDistancePerTimeUnit getBaseUnit() {
    return (VoltagePerDistancePerTimeUnit) super.getBaseUnit();
  }

  @Override
  public VoltagePerDistancePerTime of(double magnitude) {
    return new ImmutableVoltagePerDistancePerTime(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public VoltagePerDistancePerTime ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableVoltagePerDistancePerTime(
        fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public VoltagePerDistancePerTime zero() {
    return (VoltagePerDistancePerTime) super.zero();
  }

  @Override
  public VoltagePerDistancePerTime one() {
    return (VoltagePerDistancePerTime) super.one();
  }

  @Override
  public MutVoltagePerDistancePerTime mutable(double initialMagnitude) {
    return new MutVoltagePerDistancePerTime(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<VoltagePerDistancePerTimeUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, VoltagePerDistancePerTimeUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
