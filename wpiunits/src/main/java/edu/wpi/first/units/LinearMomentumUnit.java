// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableLinearMomentum;
import edu.wpi.first.units.measure.LinearMomentum;
import edu.wpi.first.units.measure.MutLinearMomentum;

/** A unit of linear momentum like {@link edu.wpi.first.units.Units#KilogramMetersPerSecond}. */
public final class LinearMomentumUnit extends MultUnit<MassUnit, LinearVelocityUnit> {
  private static final CombinatoryUnitCache<MassUnit, LinearVelocityUnit, LinearMomentumUnit>
      cache = new CombinatoryUnitCache<>(LinearMomentumUnit::new);

  LinearMomentumUnit(MassUnit unit, LinearVelocityUnit linearVelocityUnit) {
    super(unit, linearVelocityUnit);
  }

  LinearMomentumUnit(
      MultUnit<MassUnit, LinearVelocityUnit> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a mass and linear velocity unit to form a combined linear momentum unit.
   *
   * @param mass the unit of mass
   * @param velocity the unit of velocity
   * @return the combined unit of momentum
   */
  public static LinearMomentumUnit combine(MassUnit mass, LinearVelocityUnit velocity) {
    return cache.combine(mass, velocity);
  }

  @Override
  public LinearMomentum of(double magnitude) {
    return new ImmutableLinearMomentum(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public LinearMomentum ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableLinearMomentum(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public MutLinearMomentum mutable(double initialMagnitude) {
    return new MutLinearMomentum(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  @Override
  public VelocityUnit<LinearMomentumUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }

  /**
   * Multiplies this unit by distance to form a unit of angular momentum.
   *
   * @param distance the unit of distance
   * @return the combined unit of angular momentum
   */
  public AngularMomentumUnit mult(DistanceUnit distance) {
    return AngularMomentumUnit.combine(this, distance);
  }
}
