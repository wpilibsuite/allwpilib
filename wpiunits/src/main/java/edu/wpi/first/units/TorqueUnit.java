// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.immutable.ImmutableTorque;
import edu.wpi.first.units.mutable.MutTorque;

public class TorqueUnit extends MultUnit<DistanceUnit, ForceUnit> {
  private static final CombinatoryUnitCache<DistanceUnit, ForceUnit, TorqueUnit> cache =
      new CombinatoryUnitCache<>(TorqueUnit::new);

  protected TorqueUnit(DistanceUnit distanceUnit, ForceUnit forceUnit) {
    super(distanceUnit, forceUnit);
  }

  TorqueUnit(
      MultUnit<DistanceUnit, ForceUnit> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public static TorqueUnit combine(DistanceUnit distance, ForceUnit force) {
    return cache.combine(distance, force);
  }

  @Override
  public Torque of(double magnitude) {
    return new ImmutableTorque(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Torque ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableTorque(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public MutTorque mutable(double initialMagnitude) {
    return new MutTorque(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }
}
