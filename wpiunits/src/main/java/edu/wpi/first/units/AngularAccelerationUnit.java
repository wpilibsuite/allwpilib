// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.immutable.ImmutableAngularAcceleration;
import edu.wpi.first.units.measure.AngularAcceleration;
import edu.wpi.first.units.mutable.MutAngularAcceleration;

public class AngularAccelerationUnit extends PerUnit<AngularVelocityUnit, TimeUnit> {
  private static final CombinatoryUnitCache<AngularVelocityUnit, TimeUnit, AngularAccelerationUnit>
      cache = new CombinatoryUnitCache<>(AngularAccelerationUnit::new);

  protected AngularAccelerationUnit(AngularVelocityUnit velocity, TimeUnit period) {
    super(velocity, period);
  }

  protected AngularAccelerationUnit(
      AngularAccelerationUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public static AngularAccelerationUnit combine(AngularVelocityUnit velocity, TimeUnit period) {
    return cache.combine(velocity, period);
  }

  @Override
  public AngularAcceleration of(double magnitude) {
    return new ImmutableAngularAcceleration(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public AngularAcceleration ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableAngularAcceleration(
        fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public MutAngularAcceleration mutable(double initialMagnitude) {
    return new MutAngularAcceleration(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }
}
