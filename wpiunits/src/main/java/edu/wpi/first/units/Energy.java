// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Joules;
import static edu.wpi.first.units.Units.Watts;

public interface Energy extends Measure<EnergyUnit> {
  MathHelper<EnergyUnit, Energy> mathHelper = new MathHelper<>(Joules::of);

  @Override
  Energy copy();

  default Energy plus(Energy other) {
    return mathHelper.add(this, other);
  }

  default Energy minus(Energy other) {
    return mathHelper.minus(this, other);
  }

  default Dimensionless divide(Energy other) {
    return mathHelper.divide(this, other);
  }

  default Power divide(Time period) {
    return mathHelper.divide(this, period, Watts::of);
  }
}
