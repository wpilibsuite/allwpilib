// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;

import edu.wpi.first.units.LinearAccelerationUnit;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Measure;

public interface LinearAcceleration extends Measure<LinearAccelerationUnit> {
  MathHelper<LinearAccelerationUnit, LinearAcceleration> mathHelper =
      new MathHelper<>(MetersPerSecondPerSecond::of);

  @Override
  LinearAcceleration copy();

  @Override
  LinearAccelerationUnit unit();

  default double in(LinearAccelerationUnit otherUnit) {
    return otherUnit.fromBaseUnits(baseUnitMagnitude());
  }

  default LinearVelocity times(Time time) {
    return mathHelper.multiply(this, time, MetersPerSecond::of);
  }

  default LinearAcceleration times(Dimensionless multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  default LinearAcceleration times(double multiplier) {
    return mathHelper.multiply(this, multiplier);
  }

  default LinearAcceleration divide(Dimensionless divisor) {
    return mathHelper.divide(this, divisor);
  }

  default LinearAcceleration divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  default Dimensionless divide(LinearAcceleration divisor) {
    return mathHelper.divide(this, divisor);
  }

  default LinearAcceleration minus(LinearAcceleration other) {
    return mathHelper.minus(this, other);
  }

  default LinearAcceleration plus(LinearAcceleration other) {
    return mathHelper.add(this, other);
  }
}
