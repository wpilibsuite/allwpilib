// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;

import edu.wpi.first.units.AngularAccelerationUnit;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Measure;

public interface AngularAcceleration extends Measure<AngularAccelerationUnit> {
  MathHelper<AngularAccelerationUnit, AngularAcceleration> mathHelper =
      new MathHelper<>(RadiansPerSecondPerSecond::of);

  @Override
  AngularAcceleration copy();

  @Override
  AngularAccelerationUnit unit();

  default double in(AngularAccelerationUnit otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude());
  }

  default AngularAcceleration plus(AngularAcceleration other) {
    return mathHelper.add(this, other);
  }

  default AngularAcceleration minus(AngularAcceleration other) {
    return mathHelper.minus(this, other);
  }

  default Dimensionless divide(AngularAcceleration divisor) {
    return mathHelper.divide(this, divisor);
  }

  default AngularAcceleration divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  default AngularAcceleration divide(Dimensionless divisor) {
    return mathHelper.divide(this, divisor);
  }

  default AngularAcceleration times(double multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  default AngularAcceleration times(Dimensionless multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  default AngularVelocity times(Time time) {
    return mathHelper.multiply(this, time, RadiansPerSecond::of);
  }
}
