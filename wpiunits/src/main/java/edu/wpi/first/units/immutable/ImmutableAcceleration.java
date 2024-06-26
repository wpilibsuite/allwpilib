// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.AccelerationUnit;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.measure.Acceleration;

public final class ImmutableAcceleration<D extends Unit> implements Acceleration<D> {
  private final MathHelper<AccelerationUnit<D>, Acceleration<D>> mathHelper =
      new MathHelper<>(baseUnit()::of);

  private final double magnitude;
  private final double baseUnitMagnitude;
  private final AccelerationUnit<D> unit;

  public ImmutableAcceleration(
      double magnitude, double baseUnitMagnitude, AccelerationUnit<D> unit) {
    this.magnitude = magnitude;
    this.baseUnitMagnitude = baseUnitMagnitude;
    this.unit = unit;
  }

  public MathHelper<AccelerationUnit<D>, Acceleration<D>> getMathHelper() {
    return mathHelper;
  }

  @Override
  public double magnitude() {
    return magnitude;
  }

  @Override
  public double baseUnitMagnitude() {
    return baseUnitMagnitude;
  }

  @Override
  public AccelerationUnit<D> unit() {
    return unit;
  }

  @Override
  public Acceleration<D> copy() {
    return this;
  }

  @Override
  public String toString() {
    return toShortString();
  }
}
