// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.AccelerationUnit;
import edu.wpi.first.units.DistanceUnit;
import edu.wpi.first.units.LinearAcceleration;
import edu.wpi.first.units.LinearAccelerationUnit;
import edu.wpi.first.units.MutableMeasure;

public class MutLinearAcceleration extends LinearAcceleration
    implements MutableMeasure<
        AccelerationUnit<DistanceUnit>, LinearAcceleration, MutLinearAcceleration> {
  public MutLinearAcceleration(
      double magnitude, double baseUnitMagnitude, LinearAccelerationUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutLinearAcceleration mut_replace(
      double magnitude, AccelerationUnit<DistanceUnit> newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public LinearAcceleration copy() {
    return new LinearAcceleration(magnitude, baseUnitMagnitude, unit());
  }
}
