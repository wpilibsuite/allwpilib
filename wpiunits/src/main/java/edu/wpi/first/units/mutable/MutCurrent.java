// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Current;
import edu.wpi.first.units.CurrentUnit;
import edu.wpi.first.units.MutableMeasure;

public class MutCurrent extends Current
    implements MutableMeasure<CurrentUnit, Current, MutCurrent> {
  public MutCurrent(double magnitude, double baseUnitMagnitude, CurrentUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutCurrent mut_replace(double magnitude, CurrentUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public Current copy() {
    return new Current(magnitude, baseUnitMagnitude, unit);
  }
}
