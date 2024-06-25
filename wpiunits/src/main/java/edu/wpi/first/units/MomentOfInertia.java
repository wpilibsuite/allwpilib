// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class MomentOfInertia extends MeasureBase<MomentOfInertiaUnit> {
  protected MomentOfInertia(double magnitude, double baseUnitMagnitude, MomentOfInertiaUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Measure<MomentOfInertiaUnit> copy() {
    return this;
  }
}
