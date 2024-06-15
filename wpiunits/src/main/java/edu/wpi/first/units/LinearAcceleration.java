// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;

public class LinearAcceleration extends Acceleration<DistanceUnit> {
  private static final MathHelper<AccelerationUnit<DistanceUnit>, LinearAcceleration> mathHelper =
      new MathHelper<>(MetersPerSecondPerSecond::of);

  public LinearAcceleration(
      double magnitude, double baseUnitMagnitude, LinearAccelerationUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public LinearAcceleration copy() {
    return this;
  }

  public double in(LinearAccelerationUnit otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude);
  }

  @Override
  public LinearAccelerationUnit unit() {
    return (LinearAccelerationUnit) unit;
  }

  @Override
  public String toString() {
    return "LinearAcceleration["
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
}
