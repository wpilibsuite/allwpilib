// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public record Power(double magnitude, double baseUnitMagnitude, PowerUnit unit)
    implements Measure<PowerUnit> {
  @Override
  public Power copy() {
    return this;
  }

  public double in(Per<EnergyUnit, TimeUnit> otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude);
  }
}
