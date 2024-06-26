// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.MomentOfInertia;
import edu.wpi.first.units.MomentOfInertiaUnit;
import edu.wpi.first.units.immutable.ImmutableMomentOfInertia;

public final class MutMomentOfInertia
    extends MutableMeasureBase<MomentOfInertiaUnit, MomentOfInertia, MutMomentOfInertia>
    implements MomentOfInertia {
  public MutMomentOfInertia(double magnitude, double baseUnitMagnitude, MomentOfInertiaUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MomentOfInertia copy() {
    return new ImmutableMomentOfInertia(magnitude(), baseUnitMagnitude(), unit());
  }
}
