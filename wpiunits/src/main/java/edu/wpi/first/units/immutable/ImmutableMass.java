// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.MassUnit;
import edu.wpi.first.units.measure.Mass;

public record ImmutableMass(double magnitude, double baseUnitMagnitude, MassUnit unit)
    implements Mass {
  @Override
  public Mass copy() {
    return this;
  }

  @Override
  public String toString() {
    return toShortString();
  }
}
