// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Torque;
import edu.wpi.first.units.TorqueUnit;

public class MutTorque extends Torque implements MutableMeasure<TorqueUnit, Torque, MutTorque> {
  protected MutTorque(double magnitude, double baseUnitMagnitude, TorqueUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutTorque mut_replace(double magnitude, TorqueUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public Torque copy() {
    return new Torque(magnitude, baseUnitMagnitude, unit);
  }
}
