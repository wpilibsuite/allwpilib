// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class AngularAccelerationUnit extends AccelerationUnit<AngleUnit> {
  private static final CombinatoryUnitCache<
          VelocityUnit<AngleUnit>, TimeUnit, AngularAccelerationUnit>
      cache = new CombinatoryUnitCache<>(AngularAccelerationUnit::new);

  protected AngularAccelerationUnit(VelocityUnit<AngleUnit> velocity, TimeUnit period) {
    super(velocity, period);
  }

  protected AngularAccelerationUnit(
      AccelerationUnit<AngleUnit> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public static AngularAccelerationUnit combine(AngularVelocityUnit velocity, TimeUnit period) {
    return cache.combine(velocity, period);
  }

  public AngularAcceleration of(double magnitude) {
    return new AngularAcceleration(magnitude, toBaseUnits(magnitude), this);
  }

  public AngularAcceleration.Mutable mutable(double initialMagnitude) {
    return new AngularAcceleration.Mutable(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }
}
