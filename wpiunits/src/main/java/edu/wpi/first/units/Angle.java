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
// eg Mass * Distance * AngularVelocity is equivalent to (Mass * Distance) / Time - otherwise known
// as Power - in other words, AngularVelocity is /actually/ Frequency
public class Angle extends Unit<Angle> {
  Angle(Angle baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  Angle(
      Angle baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Creates a angular velocity unit derived from this one. Can be chained to denote angular
   * velocity, angular acceleration, angular jerk, etc.
   *
   * <pre>
   *   Radians.per(Second) // angular velocity
   *   Degrees.per(Second).per(Second).of(90) // 90 degrees per second
   * </pre>
   *
   * @param period the time period of the velocity, such as seconds or milliseconds
   * @return a linear velocity unit corresponding to the rate of change of distance over time
   */
  public AngularVelocity per2(Time period) {
    return AngularVelocity.combine(this, period);
  }
}
