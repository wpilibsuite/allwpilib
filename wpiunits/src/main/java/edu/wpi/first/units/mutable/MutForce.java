// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Force;
import edu.wpi.first.units.ForceUnit;
import edu.wpi.first.units.MutableMeasure;

public class MutForce extends Force implements MutableMeasure<ForceUnit, Force, MutForce> {
  public MutForce(double magnitude, double baseUnitMagnitude, ForceUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutForce mut_replace(double magnitude, ForceUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);
    return this;
  }

  @Override
  public Force copy() {
    return new Force(magnitude, baseUnitMagnitude, unit);
  }
}
