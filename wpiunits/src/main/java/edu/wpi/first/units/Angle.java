// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of angular dimension.
 *
 * <p>This is the base type for units of angular dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;Angle&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Degrees} and {@link Units#Radians}) can be found in the
 * {@link Units} class.
 */
// technically, angles are unitless dimensions
// eg Mass * Distance * Velocity<Angle> is equivalent to (Mass * Distance) / Time - otherwise known
// as Power - in other words, Velocity<Angle> is /actually/ Frequency
public class Angle extends Unit<Angle> {
  /**
   * Creates a new unit with the given name and multiplier to the base unit.
   *
   * @param baseUnitEquivalent the multiplier to convert this unit to the base unit of this type
   * @param name the name of the angle measure
   * @param symbol the symbol of the angle measure
   */
  Angle(double baseUnitEquivalent, String name, String symbol) {
    super(Angle.class, baseUnitEquivalent, name, symbol);
  }

  Angle(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Angle.class, toBaseConverter, fromBaseConverter, name, symbol);
  }
}
