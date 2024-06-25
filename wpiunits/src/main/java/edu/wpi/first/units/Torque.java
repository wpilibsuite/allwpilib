// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Torque extends MeasureBase<TorqueUnit> {
  public Torque(double magnitude, double baseUnitMagnitude, TorqueUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Torque copy() {
    return this;
  }
}
