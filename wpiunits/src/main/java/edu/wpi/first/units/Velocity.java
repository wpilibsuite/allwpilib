// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import java.util.Objects;

public class Velocity<D extends Unit> implements Measure<VelocityUnit<D>> {
  protected double magnitude;
  protected double baseUnitMagnitude;
  protected VelocityUnit<D> unit;

  public Velocity(double magnitude, double baseUnitMagnitude, VelocityUnit<D> unit) {
    this.magnitude = magnitude;
    this.baseUnitMagnitude = baseUnitMagnitude;
    this.unit = unit;
  }

  @Override
  public Velocity<D> copy() {
    return this;
  }

  public double in(VelocityUnit<? extends D> otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude);
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
  public VelocityUnit<D> unit() {
    return unit;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == this) return true;
    if (obj == null || obj.getClass() != this.getClass()) return false;
    var that = (Velocity) obj;
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
    return "Velocity["
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

  public static class Mutable<D extends Unit> extends Velocity<D>
      implements MutableMeasure<VelocityUnit<D>, Velocity<D>, Mutable<D>> {
    public Mutable(double magnitude, double baseUnitMagnitude, VelocityUnit<D> unit) {
      super(magnitude, baseUnitMagnitude, unit);
    }

    @Override
    public Mutable<D> mut_replace(double magnitude, VelocityUnit<D> newUnit) {
      this.unit = newUnit;
      this.magnitude = magnitude;
      this.baseUnitMagnitude = unit.toBaseUnits(magnitude);
      return this;
    }
  }
}
