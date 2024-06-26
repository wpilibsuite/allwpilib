// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.AngularAccelerationUnit;
import edu.wpi.first.units.immutable.ImmutableAngularAcceleration;
import edu.wpi.first.units.measure.AngularAcceleration;

public final class MutAngularAcceleration
    extends MutableMeasureBase<AngularAccelerationUnit, AngularAcceleration, MutAngularAcceleration>
    implements AngularAcceleration {
  public MutAngularAcceleration(
      double magnitude, double baseUnitMagnitude, AngularAccelerationUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public AngularAcceleration copy() {
    return new ImmutableAngularAcceleration(magnitude(), baseUnitMagnitude(), unit());
  }
}
