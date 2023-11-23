// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of electic current dimension.
 *
 * <p>This is the base type for units of current dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;Current&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Amps} and {@link Units#Milliamps}) can be found in the
 * {@link Units} class.
 */
public class Current extends Unit<Current> {
  Current(double baseUnitEquivalent, String name, String symbol) {
    super(Current.class, baseUnitEquivalent, name, symbol);
  }

  Current(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Current.class, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public Power times(Unit<Voltage> voltage, String name, String symbol) {
    return new Power(this.toBaseUnits(1) * voltage.toBaseUnits(1), name, symbol);
  }
}
