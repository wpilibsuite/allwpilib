// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Hertz;
import static edu.wpi.first.units.Units.Seconds;

public class Frequency extends MeasureBase<FrequencyUnit> {
  private static final MathHelper<FrequencyUnit, Frequency> mathHelper =
      new MathHelper<>(Hertz::of);

  public Frequency(double magnitude, double baseUnitMagnitude, FrequencyUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Frequency copy() {
    return this;
  }

  public Dimensionless divide(Frequency other) {
    return mathHelper.divide(this, other);
  }

  public Time inverse() {
    return Seconds.of(1 / baseUnitMagnitude);
  }
}
