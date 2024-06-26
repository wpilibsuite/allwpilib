// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;

public interface LinearAcceleration extends Acceleration<DistanceUnit> {
  MathHelper<AccelerationUnit<DistanceUnit>, LinearAcceleration> mathHelper =
      new MathHelper<>(MetersPerSecondPerSecond::of);

  @Override
  LinearAcceleration copy();

  @Override
  LinearAccelerationUnit unit();

  default double in(LinearAccelerationUnit otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude());
  }

  @Override
  default LinearVelocity times(Time time) {
    return mathHelper.multiply(this, time, MetersPerSecond::of);
  }

  @Override
  default LinearAcceleration times(Dimensionless multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  @Override
  default LinearAcceleration times(double multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  @Override
  default LinearAcceleration divide(Dimensionless divisor) {
    return mathHelper.divide(this, divisor);
  }

  @Override
  default LinearAcceleration divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  @Override
  default Dimensionless divide(Acceleration<DistanceUnit> divisor) {
    return mathHelper.divide(this, divisor);
  }

  @Override
  default Acceleration<DistanceUnit> minus(Acceleration<DistanceUnit> other) {
    return mathHelper.minus(this, other);
  }

  @Override
  default Acceleration<DistanceUnit> plus(Acceleration<DistanceUnit> other) {
    return mathHelper.add(this, other);
  }
}
