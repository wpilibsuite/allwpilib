// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.LinearVelocityUnit;
import edu.wpi.first.units.immutable.ImmutableLinearVelocity;
import edu.wpi.first.units.measure.LinearVelocity;

public final class MutLinearVelocity
    extends MutableMeasureBase<LinearVelocityUnit, LinearVelocity, MutLinearVelocity>
    implements LinearVelocity {
  public MutLinearVelocity(double magnitude, double baseUnitMagnitude, LinearVelocityUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public LinearVelocity copy() {
    return new ImmutableLinearVelocity(magnitude, baseUnitMagnitude, unit());
  }
}
