// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.AngularMomentumUnit;
import edu.wpi.first.units.immutable.ImmutableAngularMomentum;
import edu.wpi.first.units.measure.AngularMomentum;

public final class MutAngularMomentum
    extends MutableMeasureBase<AngularMomentumUnit, AngularMomentum, MutAngularMomentum>
    implements AngularMomentum {
  public MutAngularMomentum(double magnitude, double baseUnitMagnitude, AngularMomentumUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public AngularMomentum copy() {
    return new ImmutableAngularMomentum(magnitude, baseUnitMagnitude, unit);
  }
}
