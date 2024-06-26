// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.LinearAcceleration;
import edu.wpi.first.units.LinearAccelerationUnit;

public record ImmutableLinearAcceleration(
    double magnitude, double baseUnitMagnitude, LinearAccelerationUnit unit)
    implements LinearAcceleration {
  @Override
  public LinearAcceleration copy() {
    return this;
  }
}
