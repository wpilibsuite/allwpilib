// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;

import java.util.Objects;

public class AngularAcceleration extends Acceleration<AngleUnit> {
  private static final MathHelper<AccelerationUnit<AngleUnit>, AngularAcceleration> mathHelper =
      new MathHelper<>(RadiansPerSecondPerSecond::of);

  public AngularAcceleration(
      double magnitude, double baseUnitMagnitude, AngularAccelerationUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public AngularAcceleration copy() {
    return this;
  }

  public Dimensionless divide(AngularAcceleration other) {
    return mathHelper.divide(this, other);
  }

  public AngularVelocity times(Time period) {
    return mathHelper.multiply(this, period, RadiansPerSecond::of);
  }

  @Override
  public AngularAccelerationUnit unit() {
    return (AngularAccelerationUnit) unit;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == this) return true;
    if (obj == null || obj.getClass() != this.getClass()) return false;
    var that = (AngularAcceleration) obj;
    return Double.doubleToLongBits(this.magnitude) == Double.doubleToLongBits(that.magnitude)
        && Double.doubleToLongBits(this.baseUnitMagnitude)
            == Double.doubleToLongBits(that.baseUnitMagnitude)
        && Objects.equals(this.unit, that.unit);
  }

  @Override
  public int hashCode() {
    return Objects.hash(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public String toString() {
    return "AngularAcceleration["
        + "magnitude="
        + magnitude
        + ", "
        + "baseUnitMagnitude="
        + baseUnitMagnitude
        + ", "
        + "unit="
        + unit
        + ']';
  }

  public double in(AngularAccelerationUnit otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude);
  }

  public static class Mutable extends AngularAcceleration
      implements IMutable<AccelerationUnit<AngleUnit>, AngularAcceleration, Mutable> {
    public Mutable(double magnitude, double baseUnitMagnitude, AngularAccelerationUnit unit) {
      super(magnitude, baseUnitMagnitude, unit);
    }

    @Override
    public Mutable mut_replace(double magnitude, AccelerationUnit<AngleUnit> newUnit) {
      this.unit = newUnit;
      this.magnitude = magnitude;
      this.baseUnitMagnitude = newUnit.toBaseUnits(magnitude);
      return this;
    }
  }
}
