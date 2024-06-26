// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Kelvin;

public interface Temperature extends Measure<TemperatureUnit> {
  MathHelper<TemperatureUnit, Temperature> mathHelper = new MathHelper<>(Kelvin::of);

  @Override
  Temperature copy();

  default Velocity<TemperatureUnit> per(Time period) {
    return mathHelper.divide(
        this, period, VelocityUnit.combine(unit(), period.unit())::ofBaseUnits);
  }

  default Velocity<TemperatureUnit> per(TimeUnit period) {
    return mathHelper.divide(this, period);
  }
}
