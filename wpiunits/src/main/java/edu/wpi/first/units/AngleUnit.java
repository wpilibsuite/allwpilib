// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.units.measure.MutAngle;

/**
 * Unit of angular dimension.
 *
 * <p>This is the base type for units of angular dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;AngleUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Degrees} and {@link Units#Radians}) can be found in the
 * {@link Units} class.
 */
// technically, angles are unitless dimensions
// eg MassUnit * DistanceUnit * VelocityUnit<AngleUnit> is equivalent to (MassUnit * DistanceUnit) /
// TimeUnit - otherwise known
// as PowerUnit - in other words, VelocityUnit<AngleUnit> is /actually/ Frequency
public final class AngleUnit extends Unit {
  AngleUnit(AngleUnit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  AngleUnit(
      AngleUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  @Override
  public AngleUnit getBaseUnit() {
    return (AngleUnit) super.getBaseUnit();
  }

  @Override
  public Angle of(double magnitude) {
    return Angle.ofRelativeUnits(magnitude, this);
  }

  @Override
  public Angle ofBaseUnits(double baseUnitMagnitude) {
    return Angle.ofBaseUnits(baseUnitMagnitude, this);
  }

  @Override
  public MutAngle mutable(double initialMagnitude) {
    return new MutAngle(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  @Override
  public AngularVelocityUnit per(TimeUnit period) {
    return AngularVelocityUnit.combine(this, period);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<AngleUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another angle unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other angle unit
   * @param otherUnit the other angle unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, AngleUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  @Override
  public Angle zero() {
    return (Angle) super.zero();
  }

  @Override
  public Angle one() {
    return (Angle) super.one();
  }
}
