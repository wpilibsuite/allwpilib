// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of mass dimension.
 *
 * <p>This is the base type for units of mass dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;Mass&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Grams} and {@link Units#Pounds}) can be found in the {@link
 * Units} class.
 */
public class Mass extends Unit<Mass> {
  /** Creates a new unit with the given name and multiplier to the base unit. */
  Mass(Mass baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  Mass(
      Mass baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }
}
