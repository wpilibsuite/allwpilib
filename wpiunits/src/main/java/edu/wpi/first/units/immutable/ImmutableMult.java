// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Mult;
import edu.wpi.first.units.MultUnit;
import edu.wpi.first.units.Unit;

public record ImmutableMult<A extends Unit, B extends Unit>(
    double magnitude, double baseUnitMagnitude, MultUnit<A, B> unit) implements Mult<A, B> {
  @Override
  public Measure<MultUnit<A, B>> copy() {
    return this;
  }
}
