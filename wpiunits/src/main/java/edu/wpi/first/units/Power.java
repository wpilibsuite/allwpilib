// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of power dimension.
 *
 * <p>This is the base type for units of power dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;Power&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Watts} and {@link Units#Horsepower}) can be found in the
 * {@link Units} class.
 */
public class Power extends Unit<Power> {
  Power(double baseUnitEquivalent, String name, String symbol) {
    super(Power.class, baseUnitEquivalent, name, symbol);
  }

  Power(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Power.class, toBaseConverter, fromBaseConverter, name, symbol);
  }
}
