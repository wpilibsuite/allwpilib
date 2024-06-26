// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.AngularVelocityUnit;
import edu.wpi.first.units.measure.AngularVelocity;

public record ImmutableAngularVelocity(
    double magnitude, double baseUnitMagnitude, AngularVelocityUnit unit)
    implements AngularVelocity {
  @Override
  public AngularVelocity copy() {
    return this;
  }
}
