// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Hertz;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;

public class AngularVelocity extends Velocity<AngleUnit> {
  private static final MathHelper<VelocityUnit<AngleUnit>, AngularVelocity> mathHelper =
      new MathHelper<>(RadiansPerSecond::of);

  public AngularVelocity(double magnitude, double baseUnitMagnitude, AngularVelocityUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public AngularVelocity copy() {
    return this;
  }

  public double in(AngularVelocityUnit otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude);
  }

  public AngularAcceleration divide(Time period) {
    return mathHelper.divide(this, period, RadiansPerSecondPerSecond::of);
  }

  public Frequency asFrequency() {
    return Hertz.of(baseUnitMagnitude);
  }

  @Override
  public AngularVelocityUnit unit() {
    return (AngularVelocityUnit) unit;
  }

  @Override
  public String toString() {
    return "AngularVelocity["
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

  public static class Mutable extends AngularVelocity
      implements IMutable<VelocityUnit<AngleUnit>, AngularVelocity, Mutable> {
    public Mutable(double magnitude, double baseUnitMagnitude, AngularVelocityUnit unit) {
      super(magnitude, baseUnitMagnitude, unit);
    }

    @Override
    public AngularVelocity.Mutable mut_replace(double magnitude, VelocityUnit<AngleUnit> newUnit) {
      this.unit = newUnit;
      this.magnitude = magnitude;
      this.baseUnitMagnitude = unit.toBaseUnits(magnitude);
      return null;
    }
  }
}
