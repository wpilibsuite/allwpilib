// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.MetersPerSecond;

public interface LinearVelocity extends Velocity<DistanceUnit> {
  MathHelper<VelocityUnit<DistanceUnit>, LinearVelocity> mathHelper =
      new MathHelper<>(MetersPerSecond::of);

  @Override
  LinearVelocityUnit unit();

  @Override
  LinearVelocity copy();

  default double in(LinearVelocityUnit otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude());
  }

  default Distance times(Time period) {
    return mathHelper.multiply(this, period, unit().numerator()::ofBaseUnits);
  }
}
