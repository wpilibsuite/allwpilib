// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.CurrentUnit;
import edu.wpi.first.units.measure.Current;

public record ImmutableCurrent(double magnitude, double baseUnitMagnitude, CurrentUnit unit)
    implements Current {
  @Override
  public Current copy() {
    return this;
  }

  @Override
  public String toString() {
    return toShortString();
  }
}
