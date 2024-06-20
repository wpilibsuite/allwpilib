// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Watts;

import java.util.Objects;

public class Current implements Measure<CurrentUnit> {
  protected double magnitude;
  protected double baseUnitMagnitude;
  protected CurrentUnit unit;

  public Current(double magnitude, double baseUnitMagnitude, CurrentUnit unit) {
    this.magnitude = magnitude;
    this.baseUnitMagnitude = baseUnitMagnitude;
    this.unit = unit;
  }

  @Override
  public Current copy() {
    return this;
  }

  public Power times(Voltage voltage) {
    return Watts.of(this.baseUnitMagnitude * voltage.baseUnitMagnitude());
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
  public CurrentUnit unit() {
    return unit;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == this) return true;
    if (obj == null || obj.getClass() != this.getClass()) return false;
    var that = (Current) obj;
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
    return "Current["
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

  public static class Mutable extends Current implements MutableMeasure<CurrentUnit, Current, Mutable> {
    public Mutable(double magnitude, double baseUnitMagnitude, CurrentUnit unit) {
      super(magnitude, baseUnitMagnitude, unit);
    }

    @Override
    public Current copy() {
      return new Current(magnitude, baseUnitMagnitude, unit);
    }

    @Override
    public Mutable mut_replace(double magnitude, CurrentUnit newUnit) {
      this.unit = newUnit;
      this.magnitude = magnitude;
      this.baseUnitMagnitude = unit.toBaseUnits(magnitude);
      return this;
    }
  }
}
