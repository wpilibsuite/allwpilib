// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of temperature dimension.
 *
 * <p>This is the base type for units of temperature dimension. It is also used to specify the
 * dimension for {@link Measure}: <code>Measure&lt;Temperature&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Celsius} and {@link Units#Fahrenheit}) can be found in the
 * {@link Units} class.
 */
public class Temperature extends Unit<Temperature> {
  Temperature(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Temperature.class, toBaseConverter, fromBaseConverter, name, symbol);
  }
}
