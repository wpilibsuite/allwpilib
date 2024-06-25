// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.DistanceUnit;
import edu.wpi.first.units.LinearVelocity;
import edu.wpi.first.units.LinearVelocityUnit;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.VelocityUnit;

public class MutLinearVelocity extends LinearVelocity
    implements MutableMeasure<VelocityUnit<DistanceUnit>, LinearVelocity, MutLinearVelocity> {
  public MutLinearVelocity(double magnitude, double baseUnitMagnitude, LinearVelocityUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutLinearVelocity mut_replace(double magnitude, VelocityUnit<DistanceUnit> newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = newUnit.toBaseUnits(magnitude);
    return this;
  }

  @Override
  public LinearVelocity copy() {
    return new LinearVelocity(magnitude, baseUnitMagnitude, unit());
  }
}
