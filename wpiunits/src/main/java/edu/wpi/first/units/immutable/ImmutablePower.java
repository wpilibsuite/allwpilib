// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.PowerUnit;
import edu.wpi.first.units.measure.Power;

public record ImmutablePower(double magnitude, double baseUnitMagnitude, PowerUnit unit)
    implements Power {
  @Override
  public Power copy() {
    return this;
  }
}
