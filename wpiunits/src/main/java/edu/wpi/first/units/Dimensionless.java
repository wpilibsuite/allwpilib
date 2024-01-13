// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * A type of unit that corresponds to raw values and not any physical dimension, such as percentage.
 */
public class Dimensionless extends Unit<Dimensionless> {
  /**
   * Creates a new unit with the given name and multiplier to the base unit.
   *
   * @param baseUnitEquivalent the multiplier to convert this unit to the base unit of this type.
   * @param name the name of the unit
   * @param symbol the symbol of the unit
   */
  Dimensionless(double baseUnitEquivalent, String name, String symbol) {
    super(Dimensionless.class, baseUnitEquivalent, name, symbol);
  }

  Dimensionless(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Dimensionless.class, toBaseConverter, fromBaseConverter, name, symbol);
  }
}
