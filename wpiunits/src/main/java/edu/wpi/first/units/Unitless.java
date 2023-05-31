// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Unitless extends Unit<Unitless> {
  /**
   * Creates a new unit with the given name and multiplier to the base unit.
   *
   * @param baseUnitEquivalent the multiplier to convert this unit to the base unit of this type.
   * @param name the name of the unit
   * @param symbol the symbol of the unit
   */
  protected Unitless(double baseUnitEquivalent, String name, String symbol) {
    super(Unitless.class, baseUnitEquivalent, name, symbol);
  }

  Unitless(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Unitless.class, toBaseConverter, fromBaseConverter, name, symbol);
  }
}
