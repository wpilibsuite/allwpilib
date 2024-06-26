// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.immutable.ImmutableLinearMomentum;
import edu.wpi.first.units.mutable.MutLinearMomentum;

public class LinearMomentumUnit extends MultUnit<MassUnit, LinearVelocityUnit> {
  private static final CombinatoryUnitCache<MassUnit, LinearVelocityUnit, LinearMomentumUnit>
      cache = new CombinatoryUnitCache<>(LinearMomentumUnit::new);

  protected LinearMomentumUnit(MassUnit unit, LinearVelocityUnit linearVelocityUnit) {
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

  public AngularMomentumUnit mult(DistanceUnit distance) {
    return AngularMomentumUnit.combine(this, distance);
  }
}
