// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class LinearAccelerationUnit extends AccelerationUnit<DistanceUnit> {
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
    return new LinearAcceleration(magnitude, toBaseUnits(magnitude), this);
  }

  public LinearVelocityUnit getUnit() {
    return (LinearVelocityUnit) numerator();
  }

  public TimeUnit getPeriod() {
    return denominator();
  }
}
