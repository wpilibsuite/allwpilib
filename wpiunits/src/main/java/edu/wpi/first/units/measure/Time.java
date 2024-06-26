// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.Hertz;
import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.TimeUnit;

public interface Time extends Measure<TimeUnit> {
  MathHelper<TimeUnit, Time> mathHelper = new MathHelper<>(Seconds::of);

  @Override
  Time copy();

  default Frequency inverse() {
    return Hertz.of(1 / baseUnitMagnitude());
  }

  default Frequency asFrequency() {
    return inverse();
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
