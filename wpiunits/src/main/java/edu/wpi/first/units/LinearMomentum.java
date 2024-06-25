// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class LinearMomentum extends MeasureBase<LinearMomentumUnit> {
  public LinearMomentum(double magnitude, double baseUnitMagnitude, LinearMomentumUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public LinearMomentum copy() {
    return this;
  }
}
