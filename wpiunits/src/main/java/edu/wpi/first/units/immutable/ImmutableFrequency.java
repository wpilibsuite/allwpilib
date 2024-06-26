// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.Frequency;
import edu.wpi.first.units.FrequencyUnit;

public record ImmutableFrequency(double magnitude, double baseUnitMagnitude, FrequencyUnit unit)
    implements Frequency {
  @Override
  public Frequency copy() {
    return this;
  }
}
