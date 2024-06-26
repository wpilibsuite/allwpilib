// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.LinearVelocityUnit;
import edu.wpi.first.units.measure.LinearVelocity;

public record ImmutableLinearVelocity(
    double magnitude, double baseUnitMagnitude, LinearVelocityUnit unit) implements LinearVelocity {
  @Override
  public LinearVelocity copy() {
    return this;
  }

  @Override
  public String toString() {
    return toShortString();
  }
}
