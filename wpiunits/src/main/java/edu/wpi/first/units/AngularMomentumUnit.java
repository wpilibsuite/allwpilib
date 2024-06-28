// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.AngularMomentum;
import edu.wpi.first.units.measure.ImmutableAngularMomentum;
import edu.wpi.first.units.measure.MutAngularMomentum;

public class AngularMomentumUnit extends MultUnit<LinearMomentumUnit, DistanceUnit> {
  private static final CombinatoryUnitCache<LinearMomentumUnit, DistanceUnit, AngularMomentumUnit>
      cache = new CombinatoryUnitCache<>(AngularMomentumUnit::new);

  protected AngularMomentumUnit(LinearMomentumUnit momentumUnit, DistanceUnit distanceUnit) {
    super(momentumUnit, distanceUnit);
  }

  AngularMomentumUnit(
      AngularMomentumUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public static AngularMomentumUnit combine(LinearMomentumUnit linear, DistanceUnit distance) {
    return cache.combine(linear, distance);
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
  public MutAngularMomentum mutable(double magnitude) {
    return new MutAngularMomentum(magnitude, toBaseUnits(magnitude), this);
  }

  public MomentOfInertiaUnit mult(AngularVelocityUnit omega) {
    return MomentOfInertiaUnit.combine(this, omega);
  }
}
