// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Time;
import edu.wpi.first.units.TimeUnit;
import edu.wpi.first.units.immutable.ImmutableTime;

public final class MutTime extends MutableMeasureBase<TimeUnit, Time, MutTime> implements Time {
  public MutTime(double magnitude, double baseUnitMagnitude, TimeUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Time copy() {
    return new ImmutableTime(magnitude, baseUnitMagnitude, unit);
  }
}
