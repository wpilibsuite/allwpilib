// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of frequency dimension.
 *
 * <p>This is the base type for units of frequency dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;Frequency&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Hertz}) can be found in
 * the {@link Units} class.
 */
public class Frequency extends Unit<Frequency> {
    Frequency(Frequency baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  Frequency(
      Frequency baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }
}
