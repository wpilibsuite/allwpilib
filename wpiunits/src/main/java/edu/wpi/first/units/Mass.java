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

  /**
   * Creates a force unit derived from this one.
   *
   * <pre>
   *   Kilograms.times(MetersPerSecondPerSecond) // force
   *   Grams.times(FeetPerSecondPerSecond) // force
   * </pre>
   *
   * @param acceleration the linear acceleration acting on the mass
   * @return a force unit corresponding to mass times linear acceleration
   */
  public Force times(LinearAcceleration acceleration) {
    return Force.combine(this, acceleration);
  }

  /**
   * Creates a linear momentum unit derived from this one.
   *
   * <pre>
   *   Kilograms.times(MetersPerSecond) // linear momentum
   * </pre>
   *
   * @param velocity the linear velocity acting on the mass
   * @return a linear momentum unit corresponding to mass times linear velocity
   */
  public LinearMomentum times(LinearVelocity velocity) {
    return LinearMomentum.combine(this, velocity);
  }
}
