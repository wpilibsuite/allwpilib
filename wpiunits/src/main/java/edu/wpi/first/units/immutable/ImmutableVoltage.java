// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.Voltage;
import edu.wpi.first.units.VoltageUnit;

public record ImmutableVoltage(double magnitude, double baseUnitMagnitude, VoltageUnit unit)
    implements Voltage {
  @Override
  public Voltage copy() {
    return this;
  }
}
