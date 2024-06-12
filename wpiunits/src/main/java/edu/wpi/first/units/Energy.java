// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of energy dimension.
 *
 * <p>This is the base type for units of energy dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;Energy&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Joules} and {@link Units#Kilojoules}) can be found in the
 * {@link Units} class.
 */
public class Energy extends Unit<Energy> {
  Energy(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Energy.class, toBaseConverter, fromBaseConverter, name, symbol);
  }

  Energy(double baseUnitEquivalent, String name, String symbol) {
    super(Energy.class, baseUnitEquivalent, name, symbol);
  }
}
