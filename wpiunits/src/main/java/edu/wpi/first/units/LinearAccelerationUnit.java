// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableLinearAcceleration;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.units.measure.MutLinearAcceleration;

/**
 * A unit of linear acceleration like {@link edu.wpi.first.units.Units#MetersPerSecondPerSecond}.
 */
public final class LinearAccelerationUnit extends PerUnit<LinearVelocityUnit, TimeUnit> {
  private static final CombinatoryUnitCache<LinearVelocityUnit, TimeUnit, LinearAccelerationUnit>
      cache = new CombinatoryUnitCache<>(LinearAccelerationUnit::new);

  LinearAccelerationUnit(LinearVelocityUnit numerator, TimeUnit denominator) {
    super(
        numerator.isBaseUnit() && denominator.isBaseUnit()
            ? null
            : combine(numerator.getBaseUnit(), denominator.getBaseUnit()),
        numerator,
        denominator);
  }

  LinearAccelerationUnit(
      LinearAccelerationUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a linear velocity and time unit to form a unit of linear acceleration.
   *
   * @param velocity the unit of linear velocity
   * @param period the unit of time
   * @return the combined unit of linear acceleration
   */
  public static LinearAccelerationUnit combine(LinearVelocityUnit velocity, TimeUnit period) {
    return cache.combine(velocity, period);
  }

  @Override
  public LinearAccelerationUnit getBaseUnit() {
    return (LinearAccelerationUnit) super.getBaseUnit();
  }

  @Override
  public LinearAcceleration of(double magnitude) {
    return new ImmutableLinearAcceleration(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public LinearAcceleration ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableLinearAcceleration(
        fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public LinearAcceleration zero() {
    return (LinearAcceleration) super.zero();
  }

  @Override
  public LinearAcceleration one() {
    return (LinearAcceleration) super.one();
  }

  @Override
  public MutLinearAcceleration mutable(double initialMagnitude) {
    return new MutLinearAcceleration(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  @Override
  public VelocityUnit<LinearAccelerationUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<LinearAccelerationUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, LinearAccelerationUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  /**
   * Gets the unit of the changing velocity. This is equivalent to {@link #numerator()} and is left
   * for historical purposes.
   *
   * @return the unit of the changing velocity
   */
  public LinearVelocityUnit getUnit() {
    return numerator();
  }

  /**
   * Gets the unit of the acceleration period (how long it takes for a measured velocity to change
   * by one unit of velocity). This is equivalent to {@link #numerator()} and is left for historical
   * purposes.
   *
   * @return the unit of the acceleration period
   */
  public TimeUnit getPeriod() {
    return denominator();
  }
}
