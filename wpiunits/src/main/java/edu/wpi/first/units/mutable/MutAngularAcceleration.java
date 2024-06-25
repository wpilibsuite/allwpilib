// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.AccelerationUnit;
import edu.wpi.first.units.AngleUnit;
import edu.wpi.first.units.AngularAcceleration;
import edu.wpi.first.units.AngularAccelerationUnit;
import edu.wpi.first.units.MutableMeasure;

public class MutAngularAcceleration extends AngularAcceleration
    implements MutableMeasure<
        AccelerationUnit<AngleUnit>, AngularAcceleration, MutAngularAcceleration> {
  public MutAngularAcceleration(
      double magnitude, double baseUnitMagnitude, AngularAccelerationUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutAngularAcceleration mut_replace(double magnitude, AccelerationUnit<AngleUnit> newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = newUnit.toBaseUnits(magnitude);
    return this;
  }

  @Override
  public AngularAcceleration copy() {
    return new AngularAcceleration(magnitude, baseUnitMagnitude, unit());
  }
}
