// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableMomentOfInertia;
import edu.wpi.first.units.measure.MomentOfInertia;
import edu.wpi.first.units.measure.MutMomentOfInertia;

/**
 * A unit of moment of inertia, like {@link edu.wpi.first.units.Units#KilogramSquareMeters}. Moments
 * of inertia describe how much an object resists being rotated, analogous to mass's resistance to
 * being accelerated along a line.
 */
public final class MomentOfInertiaUnit extends PerUnit<AngularMomentumUnit, AngularVelocityUnit> {
  private static final CombinatoryUnitCache<
          AngularMomentumUnit, AngularVelocityUnit, MomentOfInertiaUnit>
      cache = new CombinatoryUnitCache<>(MomentOfInertiaUnit::new);

  MomentOfInertiaUnit(AngularMomentumUnit numerator, AngularVelocityUnit denominator) {
    super(
        numerator.isBaseUnit() && denominator.isBaseUnit()
            ? null
            : combine(numerator.getBaseUnit(), denominator.getBaseUnit()),
        numerator,
        denominator);
  }

  MomentOfInertiaUnit(
      MomentOfInertiaUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines an angular momentum and angular velocity unit to form a moment of inertia unit.
   *
   * @param momentumUnit the unit of angular momentum
   * @param velocityUnit the unit of angular velocity
   * @return the combined moment of inertia unit
   */
  public static MomentOfInertiaUnit combine(
      AngularMomentumUnit momentumUnit, AngularVelocityUnit velocityUnit) {
    return cache.combine(momentumUnit, velocityUnit);
  }

  @Override
  public MomentOfInertiaUnit getBaseUnit() {
    return (MomentOfInertiaUnit) super.getBaseUnit();
  }

  @Override
  public MomentOfInertia of(double magnitude) {
    return new ImmutableMomentOfInertia(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public MomentOfInertia ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableMomentOfInertia(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public MomentOfInertia zero() {
    return (MomentOfInertia) super.zero();
  }

  @Override
  public MomentOfInertia one() {
    return (MomentOfInertia) super.one();
  }

  @Override
  public MutMomentOfInertia mutable(double initialMagnitude) {
    return new MutMomentOfInertia(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  @Override
  public VelocityUnit<MomentOfInertiaUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<MomentOfInertiaUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, MomentOfInertiaUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
