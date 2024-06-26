// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.immutable.ImmutableDistance;
import edu.wpi.first.units.mutable.MutDistance;

/**
 * Unit of linear dimension.
 *
 * <p>This is the base type for units of linear dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;DistanceUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Meters} and {@link Units#Inches}) can be found in the
 * {@link Units} class.
 */
public class DistanceUnit extends Unit {
  DistanceUnit(DistanceUnit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  DistanceUnit(
      DistanceUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  @Override
  public DistanceUnit getBaseUnit() {
    return (DistanceUnit) super.getBaseUnit();
  }

  public LinearVelocityUnit per(TimeUnit period) {
    return LinearVelocityUnit.combine(this, period);
  }

  public double convertFrom(double magnitude, DistanceUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  public Distance of(double magnitude) {
    return new ImmutableDistance(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Distance ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableDistance(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  // distance times force = torque
  // force times distance = energy
  public TorqueUnit mult(ForceUnit force) {
    return TorqueUnit.combine(this, force);
  }

  public MutDistance mutable(double initialMagnitude) {
    return new MutDistance(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  public Distance zero() {
    return of(0);
  }
}
