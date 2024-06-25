// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Watts;

public class Current extends MeasureBase<CurrentUnit> {
  public Current(double magnitude, double baseUnitMagnitude, CurrentUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Current copy() {
    return this;
  }

  public Power times(Voltage voltage) {
    return Watts.of(this.baseUnitMagnitude * voltage.baseUnitMagnitude());
  }
}
