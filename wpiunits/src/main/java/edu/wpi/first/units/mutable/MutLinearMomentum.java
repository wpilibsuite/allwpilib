// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.LinearMomentumUnit;
import edu.wpi.first.units.immutable.ImmutableLinearMomentum;
import edu.wpi.first.units.measure.LinearMomentum;

public final class MutLinearMomentum
    extends MutableMeasureBase<LinearMomentumUnit, LinearMomentum, MutLinearMomentum>
    implements LinearMomentum {
  public MutLinearMomentum(double magnitude, double baseUnitMagnitude, LinearMomentumUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public LinearMomentum copy() {
    return new ImmutableLinearMomentum(magnitude, baseUnitMagnitude, unit);
  }
}
