// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Time;
import edu.wpi.first.units.TimeUnit;

public class MutTime extends Time implements MutableMeasure<TimeUnit, Time, MutTime> {
  protected MutTime(double magnitude, double baseUnitMagnitude, TimeUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutTime mut_replace(double magnitude, TimeUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public Time copy() {
    return new Time(magnitude, baseUnitMagnitude, unit);
  }
}
