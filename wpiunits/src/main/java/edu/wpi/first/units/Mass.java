// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Mass extends Unit<Mass> {
  /** Creates a new unit with the given name and multiplier to the base unit. */
  Mass(double baseUnitEquivalent, String name, String symbol) {
    super(Mass.class, baseUnitEquivalent, name, symbol);
  }

  Mass(UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Mass.class, toBaseConverter, fromBaseConverter, name, symbol);
  }
}
