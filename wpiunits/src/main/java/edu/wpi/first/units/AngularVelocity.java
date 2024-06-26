// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Hertz;
import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;

public interface AngularVelocity extends Velocity<AngleUnit> {
  MathHelper<VelocityUnit<AngleUnit>, AngularVelocity> mathHelper =
      new MathHelper<>(RadiansPerSecond::of);

  @Override
  AngularVelocity copy();

  default double in(AngularVelocityUnit otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude());
  }

  default Angle times(Time period) {
    return mathHelper.multiply(this, period, Radians::of);
  }

  default AngularAcceleration divide(Time period) {
    return mathHelper.divide(this, period, RadiansPerSecondPerSecond::of);
  }

  default Frequency asFrequency() {
    return Hertz.of(baseUnitMagnitude());
  }

  @Override
  AngularVelocityUnit unit();
}
