// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.mutable.MutAngle;

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
public class AngleUnit extends Unit {
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
  public Angle of(double magnitude) {
    return new Angle(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Angle ofBaseUnits(double baseUnitMagnitude) {
    return new Angle(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  public AngularVelocityUnit per(TimeUnit second) {
    return AngularVelocityUnit.combine(this, second);
  }

  public double convertFrom(double magnitude, AngleUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  @Override
  public MutAngle mutable(double initialMagnitude) {
    return new MutAngle(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }
}
