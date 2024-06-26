// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.immutable.ImmutableLinearAcceleration;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.units.mutable.MutLinearAcceleration;

public class LinearAccelerationUnit extends PerUnit<LinearVelocityUnit, TimeUnit> {
  private static final CombinatoryUnitCache<LinearVelocityUnit, TimeUnit, LinearAccelerationUnit>
      cache = new CombinatoryUnitCache<>(LinearAccelerationUnit::new);

  protected LinearAccelerationUnit(LinearVelocityUnit numerator, TimeUnit denominator) {
    super(numerator, denominator);
  }

  LinearAccelerationUnit(
      LinearAccelerationUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

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
  public MutLinearAcceleration mutable(double initialMagnitude) {
    return new MutLinearAcceleration(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  public LinearVelocityUnit getUnit() {
    return numerator();
  }

  public TimeUnit getPeriod() {
    return denominator();
  }
}
