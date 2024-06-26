// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.AngleUnit;
import edu.wpi.first.units.measure.Angle;

public record ImmutableAngle(double magnitude, double baseUnitMagnitude, AngleUnit unit)
    implements Angle {

  @Override
  public Angle copy() {
    return this;
  }

}
