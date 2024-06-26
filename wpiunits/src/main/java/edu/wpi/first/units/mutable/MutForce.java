// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Force;
import edu.wpi.first.units.ForceUnit;
import edu.wpi.first.units.immutable.ImmutableForce;

public final class MutForce extends MutableMeasureBase<ForceUnit, Force, MutForce>
    implements Force {
  public MutForce(double magnitude, double baseUnitMagnitude, ForceUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Force copy() {
    return new ImmutableForce(magnitude, baseUnitMagnitude, unit);
  }
}
