// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Voltage;
import edu.wpi.first.units.VoltageUnit;

public class MutVoltage extends Voltage
    implements MutableMeasure<VoltageUnit, Voltage, MutVoltage> {
  public MutVoltage(double magnitude, double baseUnitMagnitude, VoltageUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  public MutVoltage mut_replace(double value, VoltageUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = value;
    this.baseUnitMagnitude = unit.toBaseUnits(value);
    return this;
  }

  @Override
  public Voltage copy() {
    return new Voltage(magnitude, baseUnitMagnitude, unit);
  }
}
