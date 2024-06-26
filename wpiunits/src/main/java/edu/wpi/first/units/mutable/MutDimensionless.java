// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.DimensionlessUnit;
import edu.wpi.first.units.immutable.ImmutableDimensionless;
import edu.wpi.first.units.measure.Dimensionless;

public final class MutDimensionless
    extends MutableMeasureBase<DimensionlessUnit, Dimensionless, MutDimensionless>
    implements Dimensionless {
  public MutDimensionless(double magnitude, double baseUnitMagnitude, DimensionlessUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Dimensionless copy() {
    return new ImmutableDimensionless(magnitude, baseUnitMagnitude, unit);
  }
}
