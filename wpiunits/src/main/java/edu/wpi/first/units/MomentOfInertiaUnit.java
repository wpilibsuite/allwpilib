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
    super(numerator, denominator);
  }

  MomentOfInertiaUnit(
      PerUnit<AngularMomentumUnit, AngularVelocityUnit> baseUnit,
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
  public MomentOfInertia of(double magnitude) {
    return new ImmutableMomentOfInertia(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public MomentOfInertia ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableMomentOfInertia(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public MutMomentOfInertia mutable(double initialMagnitude) {
    return new MutMomentOfInertia(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  @Override
  public VelocityUnit<MomentOfInertiaUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }
}
