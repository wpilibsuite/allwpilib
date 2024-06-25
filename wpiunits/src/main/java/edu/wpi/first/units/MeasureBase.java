// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import java.util.Objects;

public abstract class MeasureBase<U extends Unit> implements Measure<U> {
  protected double magnitude;
  protected double baseUnitMagnitude;
  protected U unit;

  protected MeasureBase(double magnitude, double baseUnitMagnitude, U unit) {
    this.magnitude = magnitude;
    this.baseUnitMagnitude = baseUnitMagnitude;
    this.unit = Objects.requireNonNull(unit, "Unit cannot be null");
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
  public U unit() {
    return unit;
  }

  @Override
  public String toString() {
    return toShortString();
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (!(o instanceof MeasureBase<?> that)) return false;
    return Double.compare(magnitude, that.magnitude) == 0
        && Double.compare(baseUnitMagnitude, that.baseUnitMagnitude) == 0
        && Objects.equals(unit, that.unit);
  }

  @Override
  public int hashCode() {
    return Objects.hash(magnitude, baseUnitMagnitude, unit);
  }
}
