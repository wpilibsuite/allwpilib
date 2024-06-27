// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.PerUnit;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.measure.Ratio;

public record ImmutableRatio<N extends Unit, D extends Unit>(
    double magnitude, double baseUnitMagnitude, PerUnit<N, D> unit) implements Ratio<N, D> {
  @Override
  public ImmutableRatio<N, D> copy() {
    return this;
  }
}
