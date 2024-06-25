// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Angle;
import edu.wpi.first.units.AngleUnit;
import edu.wpi.first.units.MutableMeasure;

public class MutAngle extends Angle implements MutableMeasure<AngleUnit, Angle, MutAngle> {
  public MutAngle(double magnitude, double baseUnitMagnitude, AngleUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutAngle mut_replace(double value, AngleUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = value;
    this.baseUnitMagnitude = newUnit.toBaseUnits(value);
    return this;
  }

  @Override
  public Angle copy() {
    return new Angle(magnitude, baseUnitMagnitude, unit);
  }
}
