// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.TorqueUnit;
import edu.wpi.first.units.immutable.ImmutableTorque;
import edu.wpi.first.units.measure.Torque;

public final class MutTorque extends MutableMeasureBase<TorqueUnit, Torque, MutTorque>
    implements Torque {
  public MutTorque(double magnitude, double baseUnitMagnitude, TorqueUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Torque copy() {
    return new ImmutableTorque(magnitude, baseUnitMagnitude, unit);
  }
}
