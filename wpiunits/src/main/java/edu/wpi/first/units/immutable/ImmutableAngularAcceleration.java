// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.AngularAccelerationUnit;
import edu.wpi.first.units.measure.AngularAcceleration;

public record ImmutableAngularAcceleration(
    double magnitude, double baseUnitMagnitude, AngularAccelerationUnit unit)
    implements AngularAcceleration {
  @Override
  public AngularAcceleration copy() {
    return this;
  }

  @Override
  public AngularAccelerationUnit unit() {
    return unit;
  }
}
