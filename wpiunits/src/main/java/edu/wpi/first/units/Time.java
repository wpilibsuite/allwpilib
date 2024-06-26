// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Hertz;
import static edu.wpi.first.units.Units.Seconds;

public interface Time extends Measure<TimeUnit> {
  MathHelper<TimeUnit, Time> mathHelper = new MathHelper<>(Seconds::of);

  @Override
  Time copy();

  default Frequency inverse() {
    return Hertz.of(1 / baseUnitMagnitude());
  }

  default Dimensionless divide(Time other) {
    return mathHelper.divide(this, other);
  }

  default Time plus(Time other) {
    return mathHelper.add(this, other);
  }

  default Time minus(Time other) {
    return mathHelper.minus(this, other);
  }
}
