// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.AngularAcceleration;
import edu.wpi.first.units.measure.ImmutableAngularAcceleration;
import edu.wpi.first.units.measure.MutAngularAcceleration;

/** A unit of angular acceleration, such as {@link Units#RadiansPerSecondPerSecond}. */
public final class AngularAccelerationUnit extends PerUnit<AngularVelocityUnit, TimeUnit> {
  private static final CombinatoryUnitCache<AngularVelocityUnit, TimeUnit, AngularAccelerationUnit>
      cache = new CombinatoryUnitCache<>(AngularAccelerationUnit::new);

  AngularAccelerationUnit(AngularVelocityUnit velocity, TimeUnit period) {
    super(velocity, period);
  }

  AngularAccelerationUnit(
      AngularAccelerationUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines an angular velocity and time period unit into an angular acceleration.
   *
   * @param velocity the unit of velocity
   * @param period the unit of time
   * @return the combined angular acceleration unit
   */
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

  @Override
  public VelocityUnit<AngularAccelerationUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }
}
