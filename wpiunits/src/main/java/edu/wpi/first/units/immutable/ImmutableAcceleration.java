// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.Acceleration;
import edu.wpi.first.units.AccelerationUnit;
import edu.wpi.first.units.Dimensionless;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Time;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.Velocity;
import java.util.Objects;

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

  @Override
  public String toString() {
    return toShortString();
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (!(o instanceof ImmutableAcceleration<?> that)) return false;
    return Double.compare(magnitude, that.magnitude) == 0
        && Double.compare(baseUnitMagnitude, that.baseUnitMagnitude) == 0
        && Objects.equals(unit, that.unit);
  }

  @Override
  public int hashCode() {
    return Objects.hash(magnitude, baseUnitMagnitude, unit);
  }
}
