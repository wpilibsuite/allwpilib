// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Mass;
import edu.wpi.first.units.MassUnit;
import edu.wpi.first.units.immutable.ImmutableMass;

public final class MutMass extends MutableMeasureBase<MassUnit, Mass, MutMass> implements Mass {
  public MutMass(double magnitude, double baseUnitMagnitude, MassUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Mass copy() {
    return new ImmutableMass(magnitude, baseUnitMagnitude, unit);
  }
}
