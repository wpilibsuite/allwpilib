// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.TemperatureUnit;
import edu.wpi.first.units.measure.Temperature;

public record ImmutableTemperature(double magnitude, double baseUnitMagnitude, TemperatureUnit unit)
    implements Temperature {
  @Override
  public Temperature copy() {
    return this;
  }
}
