// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.LinearAcceleration;
import edu.wpi.first.units.LinearAccelerationUnit;
import edu.wpi.first.units.immutable.ImmutableLinearAcceleration;

public final class MutLinearAcceleration
    extends MutableMeasureBase<LinearAccelerationUnit, LinearAcceleration, MutLinearAcceleration>
    implements LinearAcceleration {
  public MutLinearAcceleration(
      double magnitude, double baseUnitMagnitude, LinearAccelerationUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public LinearAcceleration copy() {
    return new ImmutableLinearAcceleration(magnitude, baseUnitMagnitude, unit());
  }
}
