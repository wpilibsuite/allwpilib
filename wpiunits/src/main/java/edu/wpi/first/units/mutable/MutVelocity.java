// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Unit;
import edu.wpi.first.units.VelocityUnit;
import edu.wpi.first.units.immutable.ImmutableVelocity;
import edu.wpi.first.units.measure.Velocity;

public class MutVelocity<D extends Unit>
    extends MutableMeasureBase<VelocityUnit<D>, Velocity<D>, MutVelocity<D>>
    implements Velocity<D> {
  public MutVelocity(double magnitude, double baseUnitMagnitude, VelocityUnit<D> unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Velocity<D> copy() {
    return new ImmutableVelocity<>(magnitude, baseUnitMagnitude, unit);
  }
}
