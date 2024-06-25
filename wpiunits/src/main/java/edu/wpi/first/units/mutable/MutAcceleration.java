// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Acceleration;
import edu.wpi.first.units.AccelerationUnit;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Unit;

public class MutAcceleration<D extends Unit> extends Acceleration<D>
    implements MutableMeasure<AccelerationUnit<D>, Acceleration<D>, MutAcceleration<D>> {
  protected MutAcceleration(double magnitude, double baseUnitMagnitude, AccelerationUnit<D> unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutAcceleration<D> mut_replace(double magnitude, AccelerationUnit<D> newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public Acceleration<D> copy() {
    return new Acceleration<>(magnitude, baseUnitMagnitude, unit);
  }
}
