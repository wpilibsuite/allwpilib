// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Hertz;
import static edu.wpi.first.units.Units.Seconds;

public record Time(double magnitude, double baseUnitMagnitude, TimeUnit unit)
    implements Measure<TimeUnit> {
  private static final MathHelper<TimeUnit, Time> mathHelper = new MathHelper<>(Seconds::of);

  @Override
  public Time copy() {
    return this;
  }

  public Frequency inverse() {
    return Hertz.of(1 / baseUnitMagnitude);
  }

  public Dimensionless divide(Time other) {
    return mathHelper.divide(this, other);
  }

  public Time plus(Time other) {
    return mathHelper.add(this, other);
  }

  public Time minus(Time other) {
    return mathHelper.minus(this, other);
  }
}
