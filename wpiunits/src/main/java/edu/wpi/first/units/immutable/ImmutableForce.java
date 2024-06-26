// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.Force;
import edu.wpi.first.units.ForceUnit;

public record ImmutableForce(double magnitude, double baseUnitMagnitude, ForceUnit unit)
    implements Force {
  @Override
  public Force copy() {
    return this;
  }
}
