// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.Hertz;
import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.FrequencyUnit;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Measure;

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

  default Time asPeriod() {
    return inverse();
  }
}
