// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Distance extends Unit<Distance> {
  /** Creates a new unit with the given name and multiplier to the base unit. */
  Distance(double baseUnitEquivalent, String name, String symbol) {
    super(Distance.class, baseUnitEquivalent, name, symbol);
  }

  Distance(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Distance.class, toBaseConverter, fromBaseConverter, name, symbol);
  }
}
