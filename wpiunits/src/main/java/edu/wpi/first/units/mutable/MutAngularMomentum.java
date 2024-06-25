// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.AngularMomentum;
import edu.wpi.first.units.AngularMomentumUnit;
import edu.wpi.first.units.MutableMeasure;

public class MutAngularMomentum extends AngularMomentum
    implements MutableMeasure<AngularMomentumUnit, AngularMomentum, MutAngularMomentum> {
  public MutAngularMomentum(double magnitude, double baseUnitMagnitude, AngularMomentumUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutAngularMomentum mut_replace(double magnitude, AngularMomentumUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);
    return this;
  }

  @Override
  public AngularMomentum copy() {
    return new AngularMomentum(magnitude, baseUnitMagnitude, unit);
  }
}
