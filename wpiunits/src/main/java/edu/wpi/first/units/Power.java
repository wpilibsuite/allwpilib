// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Power extends MeasureBase<PowerUnit> {
  public Power(double magnitude, double baseUnitMagnitude, PowerUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Power copy() {
    return this;
  }

  public double in(PerUnit<EnergyUnit, TimeUnit> otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude);
  }
}
