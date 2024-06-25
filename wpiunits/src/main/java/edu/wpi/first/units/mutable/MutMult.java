// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Mult;
import edu.wpi.first.units.MultUnit;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Unit;

public class MutMult<A extends Unit, B extends Unit> extends Mult<A, B>
    implements MutableMeasure<MultUnit<A, B>, Mult<A, B>, MutMult<A, B>> {
  public MutMult(double magnitude, double baseUnitMagnitude, MultUnit<A, B> unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutMult<A, B> mut_replace(double magnitude, MultUnit<A, B> newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public Mult<A, B> copy() {
    return new Mult<>(magnitude, baseUnitMagnitude, unit);
  }
}
