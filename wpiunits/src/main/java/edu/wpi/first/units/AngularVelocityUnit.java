// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class AngularVelocityUnit extends VelocityUnit<AngleUnit> {
  private static final CombinatoryUnitCache<AngleUnit, TimeUnit, AngularVelocityUnit> cache =
      new CombinatoryUnitCache<>(AngularVelocityUnit::new);

  protected AngularVelocityUnit(AngleUnit numerator, TimeUnit denominator) {
    super(numerator, denominator);
  }

  AngularVelocityUnit(
      AngularVelocityUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public static AngularVelocityUnit combine(AngleUnit angle, TimeUnit time) {
    return cache.combine(angle, time);
  }

  @Override
  public AngularVelocity of(double magnitude) {
    return new AngularVelocity(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public AngularAccelerationUnit per(TimeUnit period) {
    return AngularAccelerationUnit.combine(this, period);
  }

  @Override
  public AngularVelocity.Mutable mutable(double initialMagnitude) {
    return new AngularVelocity.Mutable(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }
}
