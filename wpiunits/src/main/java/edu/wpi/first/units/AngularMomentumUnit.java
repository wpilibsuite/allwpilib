// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.AngularMomentum;
import edu.wpi.first.units.measure.ImmutableAngularMomentum;
import edu.wpi.first.units.measure.MutAngularMomentum;

/**
 * A unit of angular momentum, modeled as linear momentum of an object rotating some distance away
 * from the axis of rotation.
 */
public final class AngularMomentumUnit extends MultUnit<LinearMomentumUnit, DistanceUnit> {
  private static final CombinatoryUnitCache<LinearMomentumUnit, DistanceUnit, AngularMomentumUnit>
      cache = new CombinatoryUnitCache<>(AngularMomentumUnit::new);

  AngularMomentumUnit(LinearMomentumUnit momentumUnit, DistanceUnit distanceUnit) {
    super(
        momentumUnit.isBaseUnit() && distanceUnit.isBaseUnit()
            ? null
            : combine(momentumUnit.getBaseUnit(), distanceUnit.getBaseUnit()),
        momentumUnit,
        distanceUnit);
  }

  AngularMomentumUnit(
      AngularMomentumUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a linear momentum and distance to create a unit of angular momentum.
   *
   * @param linear the linear momentum unit
   * @param distance the unit of distance from the axis of rotation
   * @return the combined angular momentum unit
   */
  public static AngularMomentumUnit combine(LinearMomentumUnit linear, DistanceUnit distance) {
    return cache.combine(linear, distance);
  }

  @Override
  public AngularMomentumUnit getBaseUnit() {
    return (AngularMomentumUnit) super.getBaseUnit();
  }

  @Override
  public AngularMomentum of(double magnitude) {
    return new ImmutableAngularMomentum(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public AngularMomentum ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableAngularMomentum(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public AngularMomentum zero() {
    return (AngularMomentum) super.zero();
  }

  @Override
  public AngularMomentum one() {
    return (AngularMomentum) super.one();
  }

  @Override
  public MutAngularMomentum mutable(double magnitude) {
    return new MutAngularMomentum(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public VelocityUnit<AngularMomentumUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<AngularMomentumUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, AngularMomentumUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  /**
   * Multiplies this angular momentum by an angular velocity to yield a unit of moment of inertia.
   *
   * @param omega the unit of angular velocity
   * @return the moment of inertia unit
   */
  public MomentOfInertiaUnit mult(AngularVelocityUnit omega) {
    return MomentOfInertiaUnit.combine(this, omega);
  }
}
