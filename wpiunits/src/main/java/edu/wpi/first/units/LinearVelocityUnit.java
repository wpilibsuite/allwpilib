// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableLinearVelocity;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.MutLinearVelocity;

/** A unit of linear velocity like {@link edu.wpi.first.units.Units#MetersPerSecond}. */
public final class LinearVelocityUnit extends PerUnit<DistanceUnit, TimeUnit> {
  private static final CombinatoryUnitCache<DistanceUnit, TimeUnit, LinearVelocityUnit> cache =
      new CombinatoryUnitCache<>(LinearVelocityUnit::new);

  LinearVelocityUnit(DistanceUnit unit, TimeUnit period) {
    super(
        unit.isBaseUnit() && period.isBaseUnit()
            ? null
            : combine(unit.getBaseUnit(), period.getBaseUnit()),
        unit,
        period);
  }

  LinearVelocityUnit(
      PerUnit<DistanceUnit, TimeUnit> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a distance and time unit for form a combined unit of velocity.
   *
   * @param distance the unit of distance
   * @param period the unit of time
   * @return the combined velocity unit
   */
  public static LinearVelocityUnit combine(DistanceUnit distance, TimeUnit period) {
    return cache.combine(distance, period);
  }

  @Override
  public LinearVelocityUnit getBaseUnit() {
    return (LinearVelocityUnit) super.getBaseUnit();
  }

  @Override
  public LinearVelocity of(double magnitude) {
    return new ImmutableLinearVelocity(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public LinearVelocity ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableLinearVelocity(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public LinearVelocity zero() {
    return (LinearVelocity) super.zero();
  }

  @Override
  public LinearVelocity one() {
    return (LinearVelocity) super.one();
  }

  @Override
  public MutLinearVelocity mutable(double value) {
    return new MutLinearVelocity(value, toBaseUnits(value), this);
  }

  /**
   * Combines this velocity with a time period of change to form a unit of acceleration.
   *
   * @param period the period of change in the velocity
   * @return the combined acceleration unit
   */
  @Override
  public LinearAccelerationUnit per(TimeUnit period) {
    return LinearAccelerationUnit.combine(this, period);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<LinearVelocityUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, LinearVelocityUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
