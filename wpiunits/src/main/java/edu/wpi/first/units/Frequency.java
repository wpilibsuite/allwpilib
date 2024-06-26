// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Hertz;
import static edu.wpi.first.units.Units.Seconds;

public interface Frequency extends Measure<FrequencyUnit> {
  MathHelper<FrequencyUnit, Frequency> mathHelper = new MathHelper<>(Hertz::of);

  @Override
  Frequency copy();

  default Dimensionless divide(Frequency other) {
    return mathHelper.divide(this, other);
  }

  default Time inverse() {
    return Seconds.of(1 / baseUnitMagnitude());
  }
}
