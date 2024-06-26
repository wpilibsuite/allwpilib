// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.Dimensionless;
import edu.wpi.first.units.DimensionlessUnit;

public record ImmutableDimensionless(
    double magnitude, double baseUnitMagnitude, DimensionlessUnit unit) implements Dimensionless {
  @Override
  public Dimensionless copy() {
    return this;
  }
}
