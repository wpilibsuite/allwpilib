// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.LinearMomentum;
import edu.wpi.first.units.LinearMomentumUnit;
import edu.wpi.first.units.MutableMeasure;

public class MutLinearMomentum extends LinearMomentum
    implements MutableMeasure<LinearMomentumUnit, LinearMomentum, MutLinearMomentum> {
  protected MutLinearMomentum(double magnitude, double baseUnitMagnitude, LinearMomentumUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutLinearMomentum mut_replace(double magnitude, LinearMomentumUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public LinearMomentum copy() {
    return new LinearMomentum(magnitude, baseUnitMagnitude, unit);
  }
}
