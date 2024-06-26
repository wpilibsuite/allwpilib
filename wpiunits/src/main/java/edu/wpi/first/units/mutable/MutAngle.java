// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.AngleUnit;
import edu.wpi.first.units.immutable.ImmutableAngle;
import edu.wpi.first.units.measure.Angle;

public final class MutAngle extends MutableMeasureBase<AngleUnit, Angle, MutAngle> {
  public MutAngle(double magnitude, double baseUnitMagnitude, AngleUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Angle copy() {
    return new ImmutableAngle(magnitude, baseUnitMagnitude, unit);
  }
}
