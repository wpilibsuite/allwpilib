// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class LinearMomentumUnit extends Mult<MassUnit, LinearVelocityUnit> {
  private static final CombinatoryUnitCache<MassUnit, LinearVelocityUnit, LinearMomentumUnit>
      cache = new CombinatoryUnitCache<>(LinearMomentumUnit::new);

  protected LinearMomentumUnit(MassUnit unit, LinearVelocityUnit linearVelocityUnit) {
    super(unit, linearVelocityUnit);
  }

  LinearMomentumUnit(
      Mult<MassUnit, LinearVelocityUnit> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public static LinearMomentumUnit combine(MassUnit mass, LinearVelocityUnit velocity) {
    return cache.combine(mass, velocity);
  }

  public LinearMomentum of(double magnitude) {
    return new LinearMomentum(magnitude, toBaseUnits(magnitude), this);
  }

  public AngularMomentumUnit mult(DistanceUnit distance) {
    return AngularMomentumUnit.combine(this, distance);
  }
}
