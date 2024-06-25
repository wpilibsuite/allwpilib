// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Distance;
import edu.wpi.first.units.DistanceUnit;
import edu.wpi.first.units.MutableMeasure;

public class MutDistance extends Distance
    implements MutableMeasure<DistanceUnit, Distance, MutDistance> {
  public MutDistance(double magnitude, double baseUnitMagnitude, DistanceUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutDistance mut_replace(double magnitude, DistanceUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);
    return this;
  }

  @Override
  public Distance copy() {
    return new Distance(magnitude, baseUnitMagnitude, unit);
  }
}
