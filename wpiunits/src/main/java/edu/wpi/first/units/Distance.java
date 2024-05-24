// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of linear dimension.
 *
 * <p>This is the base type for units of linear dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;Distance&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Meters} and {@link Units#Inches}) can be found in the
 * {@link Units} class.
 */
public class Distance extends Unit<Distance> {
  Distance(Distance baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  Distance(
      Distance baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }
}
