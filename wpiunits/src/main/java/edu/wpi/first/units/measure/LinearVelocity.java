// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.MetersPerSecond;

import edu.wpi.first.units.DistanceUnit;
import edu.wpi.first.units.LinearVelocityUnit;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.VelocityUnit;

public interface LinearVelocity extends Measure<LinearVelocityUnit> {
  MathHelper<LinearVelocityUnit, LinearVelocity> mathHelper = new MathHelper<>(MetersPerSecond::of);

  @Override
  LinearVelocityUnit unit();

  @Override
  LinearVelocity copy();

  default double in(VelocityUnit<DistanceUnit> otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude());
  }

  default Distance times(Time period) {
    return mathHelper.multiply(this, period, unit().numerator()::ofBaseUnits);
  }
}
