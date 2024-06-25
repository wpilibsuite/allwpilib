// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.AngleUnit;
import edu.wpi.first.units.AngularVelocity;
import edu.wpi.first.units.AngularVelocityUnit;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.VelocityUnit;

public class MutAngularVelocity extends AngularVelocity
    implements MutableMeasure<VelocityUnit<AngleUnit>, AngularVelocity, MutAngularVelocity> {
  public MutAngularVelocity(double magnitude, double baseUnitMagnitude, AngularVelocityUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutAngularVelocity mut_replace(double magnitude, VelocityUnit<AngleUnit> newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);
    return null;
  }

  @Override
  public AngularVelocity copy() {
    return new AngularVelocity(magnitude, baseUnitMagnitude, unit());
  }
}
