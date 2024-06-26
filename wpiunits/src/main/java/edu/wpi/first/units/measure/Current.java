// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.Watts;

import edu.wpi.first.units.CurrentUnit;
import edu.wpi.first.units.Measure;

public interface Current extends Measure<CurrentUnit> {
  @Override
  Current copy();

  default Power times(Voltage voltage) {
    return Watts.ofBaseUnits(baseUnitMagnitude() * voltage.baseUnitMagnitude());
  }
}
