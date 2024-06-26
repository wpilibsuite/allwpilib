// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.LinearMomentumUnit;
import edu.wpi.first.units.measure.LinearMomentum;

public record ImmutableLinearMomentum(
    double magnitude, double baseUnitMagnitude, LinearMomentumUnit unit) implements LinearMomentum {
  @Override
  public LinearMomentum copy() {
    return this;
  }
}
