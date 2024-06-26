// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.immutable.ImmutableLinearVelocity;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.mutable.MutLinearVelocity;

public class LinearVelocityUnit extends PerUnit<DistanceUnit, TimeUnit> {
  private static final CombinatoryUnitCache<DistanceUnit, TimeUnit, LinearVelocityUnit> cache =
      new CombinatoryUnitCache<>(LinearVelocityUnit::new);

  LinearVelocityUnit(DistanceUnit unit, TimeUnit period) {
    super(unit, period);
  }

  LinearVelocityUnit(
      PerUnit<DistanceUnit, TimeUnit> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public static LinearVelocityUnit combine(DistanceUnit distance, TimeUnit period) {
    return cache.combine(distance, period);
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
  public MutLinearVelocity mutable(double value) {
    return new MutLinearVelocity(value, toBaseUnits(value), this);
  }

  public LinearAccelerationUnit per(TimeUnit period) {
    return LinearAccelerationUnit.combine(this, period);
  }
}
