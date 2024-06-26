// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Current;
import edu.wpi.first.units.CurrentUnit;
import edu.wpi.first.units.immutable.ImmutableCurrent;

public final class MutCurrent extends MutableMeasureBase<CurrentUnit, Current, MutCurrent>
    implements Current {
  public MutCurrent(double magnitude, double baseUnitMagnitude, CurrentUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Current copy() {
    return new ImmutableCurrent(magnitude, baseUnitMagnitude, unit);
  }
}
