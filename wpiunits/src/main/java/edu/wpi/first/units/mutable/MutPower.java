// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Power;
import edu.wpi.first.units.PowerUnit;
import edu.wpi.first.units.immutable.ImmutablePower;

public final class MutPower extends MutableMeasureBase<PowerUnit, Power, MutPower>
    implements Power {
  public MutPower(double magnitude, double baseUnitMagnitude, PowerUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Power copy() {
    return new ImmutablePower(magnitude, baseUnitMagnitude, unit);
  }
}
