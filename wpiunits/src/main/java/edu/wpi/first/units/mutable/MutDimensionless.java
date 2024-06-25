// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Dimensionless;
import edu.wpi.first.units.DimensionlessUnit;
import edu.wpi.first.units.MutableMeasure;

public class MutDimensionless extends Dimensionless
    implements MutableMeasure<DimensionlessUnit, Dimensionless, MutDimensionless> {
  public MutDimensionless(double magnitude, double baseUnitMagnitude, DimensionlessUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutDimensionless mut_replace(double magnitude, DimensionlessUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public Dimensionless copy() {
    return new Dimensionless(magnitude, baseUnitMagnitude, unit);
  }
}
