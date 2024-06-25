// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.Velocity;
import edu.wpi.first.units.VelocityUnit;

public class MutVelocity<D extends Unit> extends Velocity<D>
    implements MutableMeasure<VelocityUnit<D>, Velocity<D>, MutVelocity<D>> {
  public MutVelocity(double magnitude, double baseUnitMagnitude, VelocityUnit<D> unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutVelocity<D> mut_replace(double magnitude, VelocityUnit<D> newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);
    return this;
  }

  @Override
  public Velocity<D> copy() {
    return new Velocity<>(magnitude, baseUnitMagnitude, unit);
  }
}
