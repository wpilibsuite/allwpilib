// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.AngularVelocityUnit;
import edu.wpi.first.units.immutable.ImmutableAngularVelocity;
import edu.wpi.first.units.measure.AngularVelocity;

public final class MutAngularVelocity
    extends MutableMeasureBase<AngularVelocityUnit, AngularVelocity, MutAngularVelocity>
    implements AngularVelocity {
  public MutAngularVelocity(double magnitude, double baseUnitMagnitude, AngularVelocityUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public AngularVelocity copy() {
    return new ImmutableAngularVelocity(magnitude, baseUnitMagnitude, unit());
  }
}
