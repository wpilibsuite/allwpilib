// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.AngularMomentumUnit;
import edu.wpi.first.units.measure.AngularMomentum;

public record ImmutableAngularMomentum(
    double magnitude, double baseUnitMagnitude, AngularMomentumUnit unit)
    implements AngularMomentum {
  @Override
  public AngularMomentum copy() {
    return this;
  }
}
