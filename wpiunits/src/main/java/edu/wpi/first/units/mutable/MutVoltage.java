// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.VoltageUnit;
import edu.wpi.first.units.immutable.ImmutableVoltage;
import edu.wpi.first.units.measure.Voltage;

public final class MutVoltage extends MutableMeasureBase<VoltageUnit, Voltage, MutVoltage>
    implements Voltage {
  public MutVoltage(double magnitude, double baseUnitMagnitude, VoltageUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Voltage copy() {
    return new ImmutableVoltage(magnitude, baseUnitMagnitude, unit);
  }
}
