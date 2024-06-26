// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.FrequencyUnit;
import edu.wpi.first.units.immutable.ImmutableFrequency;
import edu.wpi.first.units.measure.Frequency;

public final class MutFrequency extends MutableMeasureBase<FrequencyUnit, Frequency, MutFrequency>
    implements Frequency {
  public MutFrequency(double magnitude, double baseUnitMagnitude, FrequencyUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Frequency copy() {
    return new ImmutableFrequency(magnitude, baseUnitMagnitude, unit);
  }
}
