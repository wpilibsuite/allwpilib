// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.Second;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.PerUnit;
import edu.wpi.first.units.TimeUnit;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.VoltageUnit;

public interface Voltage extends Measure<VoltageUnit> {
  MathHelper<VoltageUnit, Voltage> mathHelper = new MathHelper<>(Volts::of);

  @Override
  Voltage copy();

  default Velocity<VoltageUnit> divide(Time period) {
    return mathHelper.divide(this, period, Volts.per(Second)::of);
  }

  default Velocity<VoltageUnit> per(TimeUnit period) {
    return mathHelper.divide(this, period);
  }

  @SuppressWarnings("unchecked")
  default <U extends Unit> Measure<U> divide(Measure<? extends PerUnit<VoltageUnit, U>> divisor) {
    return mathHelper.divide(
        this, divisor, x -> (Measure<U>) divisor.unit().denominator().ofBaseUnits(x));
  }
}
