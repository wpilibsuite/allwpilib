// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;

public interface AngularAcceleration extends Acceleration<AngleUnit> {
  MathHelper<AccelerationUnit<AngleUnit>, AngularAcceleration> mathHelper =
      new MathHelper<>(RadiansPerSecondPerSecond::of);

  @Override
  AngularAcceleration copy();

  @Override
  AngularAccelerationUnit unit();

  default double in(AngularAccelerationUnit otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude());
  }

  @Override
  default AngularAcceleration plus(Acceleration<AngleUnit> other) {
    return mathHelper.add(this, other);
  }

  @Override
  default AngularAcceleration minus(Acceleration<AngleUnit> other) {
    return mathHelper.minus(this, other);
  }

  @Override
  default Dimensionless divide(Acceleration<AngleUnit> divisor) {
    return mathHelper.divide(this, divisor);
  }

  @Override
  default AngularAcceleration divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  @Override
  default AngularAcceleration divide(Dimensionless divisor) {
    return mathHelper.divide(this, divisor);
  }

  @Override
  default AngularAcceleration times(double multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  @Override
  default AngularAcceleration times(Dimensionless multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  @Override
  default AngularVelocity times(Time time) {
    return mathHelper.multiply(this, time, RadiansPerSecond::of);
  }
}
