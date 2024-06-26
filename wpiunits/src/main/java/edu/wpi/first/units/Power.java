// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public interface Power extends Measure<PowerUnit> {
  @Override
  Power copy();

  default double in(PerUnit<EnergyUnit, TimeUnit> otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude());
  }
}
