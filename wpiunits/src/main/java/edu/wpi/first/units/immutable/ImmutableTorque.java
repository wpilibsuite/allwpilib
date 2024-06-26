// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.Torque;
import edu.wpi.first.units.TorqueUnit;

public record ImmutableTorque(double magnitude, double baseUnitMagnitude, TorqueUnit unit)
    implements Torque {
  @Override
  public Torque copy() {
    return this;
  }
}
