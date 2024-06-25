// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Velocity<D extends Unit> extends MeasureBase<VelocityUnit<D>> {
  public Velocity(double magnitude, double baseUnitMagnitude, VelocityUnit<D> unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Velocity<D> copy() {
    return this;
  }

  public double in(VelocityUnit<? extends D> otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude);
  }
}
