// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.MetersPerSecond;

public class LinearVelocity extends Velocity<DistanceUnit> {
  private static final MathHelper<VelocityUnit<DistanceUnit>, LinearVelocity> mathHelper =
      new MathHelper<>(MetersPerSecond::of);

  public LinearVelocity(double magnitude, double baseUnitMagnitude, LinearVelocityUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public LinearVelocity copy() {
    return this;
  }

  public double in(LinearVelocityUnit otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude);
  }

  public Distance times(Time period) {
    // TODO
    throw new UnsupportedOperationException();
  }

  @Override
  public LinearVelocityUnit unit() {
    return (LinearVelocityUnit) unit;
  }

  @Override
  public String toString() {
    return "LinearVelocity["
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

  public static class Mutable extends LinearVelocity
      implements MutableMeasure<VelocityUnit<DistanceUnit>, LinearVelocity, Mutable> {
    public Mutable(double magnitude, double baseUnitMagnitude, LinearVelocityUnit unit) {
      super(magnitude, baseUnitMagnitude, unit);
    }

    @Override
    public LinearVelocity.Mutable mut_replace(
        double magnitude, VelocityUnit<DistanceUnit> newUnit) {
      this.unit = newUnit;
      this.magnitude = magnitude;
      this.baseUnitMagnitude = newUnit.toBaseUnits(magnitude);
      return this;
    }
  }
}
