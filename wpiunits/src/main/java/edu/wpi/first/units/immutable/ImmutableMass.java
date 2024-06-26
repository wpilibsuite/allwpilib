// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import edu.wpi.first.units.Mass;
import edu.wpi.first.units.MassUnit;
import edu.wpi.first.units.Time;
import edu.wpi.first.units.TimeUnit;
import edu.wpi.first.units.Velocity;

public record ImmutableMass(double magnitude, double baseUnitMagnitude, MassUnit unit)
    implements Mass {
  @Override
  public Mass copy() {
    return this;
  }

  @Override
  public Velocity<MassUnit> per(Time period) {
    return new ImmutableVelocity<>(
        magnitude / period.magnitude(),
        baseUnitMagnitude / period.baseUnitMagnitude(),
        unit.per(period.unit()));
  }

  @Override
  public Velocity<MassUnit> per(TimeUnit period) {
    return new ImmutableVelocity<>(
        magnitude / period.fromBaseUnits(1),
        baseUnitMagnitude / period.toBaseUnits(1),
        unit.per(period));
  }

  @Override
  public MassUnit unit() {
    return unit;
  }
}
