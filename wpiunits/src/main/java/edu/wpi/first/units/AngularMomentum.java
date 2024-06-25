// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class AngularMomentum extends MeasureBase<AngularMomentumUnit> {
  public AngularMomentum(double magnitude, double baseUnitMagnitude, AngularMomentumUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public AngularMomentum copy() {
    return this;
  }

  @Override
  public AngularMomentumUnit unit() {
    return unit;
  }
}
