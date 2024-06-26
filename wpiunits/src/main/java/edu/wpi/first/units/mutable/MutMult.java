// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Mult;
import edu.wpi.first.units.MultUnit;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.immutable.ImmutableMult;

public final class MutMult<A extends Unit, B extends Unit>
    extends MutableMeasureBase<MultUnit<A, B>, Mult<A, B>, MutMult<A, B>> implements Mult<A, B> {
  public MutMult(double magnitude, double baseUnitMagnitude, MultUnit<A, B> unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Mult<A, B> copy() {
    return new ImmutableMult<>(magnitude, baseUnitMagnitude, unit);
  }
}
