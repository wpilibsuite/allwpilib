// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.DistanceUnit;
import edu.wpi.first.units.immutable.ImmutableDistance;
import edu.wpi.first.units.measure.Distance;

public final class MutDistance extends MutableMeasureBase<DistanceUnit, Distance, MutDistance>
    implements Distance {
  public MutDistance(double magnitude, double baseUnitMagnitude, DistanceUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Distance copy() {
    return new ImmutableDistance(magnitude, baseUnitMagnitude, unit);
  }
}
