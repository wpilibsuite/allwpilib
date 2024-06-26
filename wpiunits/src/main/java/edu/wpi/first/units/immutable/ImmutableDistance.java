// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.Distance;
import edu.wpi.first.units.DistanceUnit;

public record ImmutableDistance(double magnitude, double baseUnitMagnitude, DistanceUnit unit)
    implements Distance {
  @Override
  public Distance copy() {
    return this;
  }
}
