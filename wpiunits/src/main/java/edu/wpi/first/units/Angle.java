// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.Value;

import java.util.Objects;

public class Angle implements Measure<AngleUnit> {
  private static final MathHelper<AngleUnit, Angle> mathHelper = new MathHelper<>(Radians::of);
  protected double magnitude;
  protected double baseUnitMagnitude;
  protected AngleUnit unit;

  public Angle(double magnitude, double baseUnitMagnitude, AngleUnit unit) {
    this.magnitude = magnitude;
    this.baseUnitMagnitude = baseUnitMagnitude;
    this.unit = unit;
  }

  @Override
  public Angle copy() {
    return this;
  }

  public Dimensionless divide(Angle other) {
    return mathHelper.divide(this, other);
  }

  public Angle plus(Angle other) {
    return mathHelper.add(this, other);
  }

  public Angle minus(Angle other) {
    return mathHelper.minus(this, other);
  }

  public Angle divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  public AngularVelocity divide(Time period) {
    return mathHelper.divide(this, period, RadiansPerSecond::of);
  }

  public AngularVelocity per(TimeUnit unit) {
    return mathHelper.divide(this, unit.of(1), RadiansPerSecond::of);
  }

  public Dimensionless per(AngleUnit unit) {
    return mathHelper.divide(this, unit.of(1), Value::of);
  }

  @SuppressWarnings("unchecked")
  public <Divisor extends Unit> Measure<? extends Per<AngleUnit, Divisor>> per(Divisor divisor) {
    return Per.combine(this.baseUnit(), (Divisor) divisor.getBaseUnit())
        .of(baseUnitMagnitude / divisor.fromBaseUnits(1));
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
  public AngleUnit unit() {
    return unit;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == this) return true;
    if (obj == null || obj.getClass() != this.getClass()) return false;
    var that = (Angle) obj;
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
    return "Angle["
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

  public static class Mutable extends Angle {
    public Mutable(double magnitude, double baseUnitMagnitude, AngleUnit unit) {
      super(magnitude, baseUnitMagnitude, unit);
    }

    public Mutable mut_replace(double value, AngleUnit newUnit) {
      this.unit = newUnit;
      this.magnitude = value;
      this.baseUnitMagnitude = newUnit.toBaseUnits(value);
      return this;
    }
  }
}
