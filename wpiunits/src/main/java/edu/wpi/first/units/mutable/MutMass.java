// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Mass;
import edu.wpi.first.units.MassUnit;
import edu.wpi.first.units.MutableMeasure;

public class MutMass extends Mass implements MutableMeasure<MassUnit, Mass, MutMass> {
  protected MutMass(double magnitude, double baseUnitMagnitude, MassUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutMass mut_replace(double magnitude, MassUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public Mass copy() {
    return new Mass(magnitude, baseUnitMagnitude, unit);
  }
}
