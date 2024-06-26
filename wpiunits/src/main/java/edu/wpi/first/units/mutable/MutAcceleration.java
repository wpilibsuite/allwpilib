// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Acceleration;
import edu.wpi.first.units.AccelerationUnit;
import edu.wpi.first.units.Dimensionless;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Time;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.Velocity;
import edu.wpi.first.units.immutable.ImmutableAcceleration;

public class MutAcceleration<D extends Unit>
    extends MutableMeasureBase<AccelerationUnit<D>, Acceleration<D>, MutAcceleration<D>>
    implements Acceleration<D> {
  private final MathHelper<AccelerationUnit<D>, Acceleration<D>> mathHelper =
      new MathHelper<>(baseUnit()::of);

  public MutAcceleration(double magnitude, double baseUnitMagnitude, AccelerationUnit<D> unit) {
    super(magnitude, baseUnitMagnitude, unit);
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
    return new ImmutableAcceleration<>(magnitude(), baseUnitMagnitude(), unit());
  }

  @Override
  public Acceleration<D> plus(Acceleration<D> other) {
    return mathHelper.add(this, other);
  }

  @Override
  public Acceleration<D> minus(Acceleration<D> other) {
    return mathHelper.minus(this, other);
  }

  @Override
  public Dimensionless divide(Acceleration<D> divisor) {
    return mathHelper.divide(this, divisor);
  }

  @Override
  public Acceleration<D> divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  @Override
  public Acceleration<D> divide(Dimensionless divisor) {
    return mathHelper.divide(this, divisor);
  }

  @Override
  public Acceleration<D> times(double multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  @Override
  public Acceleration<D> times(Dimensionless multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  @Override
  public Velocity<D> times(Time time) {
    return mathHelper.multiply(this, time, baseUnit().numerator()::of);
  }
}
