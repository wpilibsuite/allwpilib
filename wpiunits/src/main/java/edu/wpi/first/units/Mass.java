// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Kilograms;

public interface Mass extends Measure<MassUnit> {
  MathHelper<MassUnit, Mass> mathHelper = new MathHelper<>(Kilograms::of);

  @Override
  Mass copy();

  default Velocity<MassUnit> per(Time period) {
    return mathHelper.divide(
        this, period, VelocityUnit.combine(unit(), period.unit())::ofBaseUnits);
  }

  default Velocity<MassUnit> per(TimeUnit period) {
    return VelocityUnit.combine(unit(), period).of(magnitude());
  }
}
