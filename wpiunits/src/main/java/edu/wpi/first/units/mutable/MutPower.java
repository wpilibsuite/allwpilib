// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Power;
import edu.wpi.first.units.PowerUnit;

public class MutPower extends Power implements MutableMeasure<PowerUnit, Power, MutPower> {
  protected MutPower(double magnitude, double baseUnitMagnitude, PowerUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutPower mut_replace(double magnitude, PowerUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public Power copy() {
    return new Power(magnitude, baseUnitMagnitude, unit);
  }
}
