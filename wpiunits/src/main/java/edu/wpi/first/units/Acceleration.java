// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Acceleration<D extends Unit> extends MeasureBase<AccelerationUnit<D>> {
  private final MathHelper<AccelerationUnit<D>, Acceleration<D>> mathHelper =
      new MathHelper<>(baseUnit()::of);

  public Acceleration(double magnitude, double baseUnitMagnitude, AccelerationUnit<D> unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Acceleration<D> copy() {
    return this;
  }

  public Acceleration<D> zero() {
    return mathHelper.zero();
  }

  public Acceleration<D> plus(Acceleration<D> other) {
    return mathHelper.add(this, other);
  }

  public Acceleration<D> minus(Acceleration<D> other) {
    return mathHelper.minus(this, other);
  }

  public Dimensionless divide(Acceleration<D> divisor) {
    return mathHelper.divide(this, divisor);
  }

  public Acceleration<D> divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  public Acceleration<D> divide(Dimensionless divisor) {
    return mathHelper.divide(this, divisor);
  }

  public Acceleration<D> times(double multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  public Acceleration<D> times(Dimensionless multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  public Velocity<D> times(Time time) {
    return mathHelper.multiply(this, time, baseUnit().numerator()::of);
  }
}
