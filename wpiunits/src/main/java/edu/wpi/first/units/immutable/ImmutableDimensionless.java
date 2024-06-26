// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.DimensionlessUnit;
import edu.wpi.first.units.measure.Dimensionless;

public record ImmutableDimensionless(
    double magnitude, double baseUnitMagnitude, DimensionlessUnit unit) implements Dimensionless {
  @Override
  public Dimensionless copy() {
    return this;
  }
}
