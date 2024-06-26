// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.TorqueUnit;

public interface Torque extends Measure<TorqueUnit> {
  @Override
  Torque copy();
}
