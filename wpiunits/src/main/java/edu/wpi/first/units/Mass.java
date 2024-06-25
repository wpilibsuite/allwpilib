// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Mass extends MeasureBase<MassUnit> {
  public Mass(double magnitude, double baseUnitMagnitude, MassUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Mass copy() {
    return this;
  }

  public Velocity<MassUnit> per(Time period) {
    return new Velocity<>(
        magnitude / period.magnitude(),
        baseUnitMagnitude / period.baseUnitMagnitude(),
        unit.per(period.unit()));
  }

  public Velocity<MassUnit> per(TimeUnit period) {
    return new Velocity<>(
        magnitude / period.fromBaseUnits(1),
        baseUnitMagnitude / period.toBaseUnits(1),
        unit.per(period));
  }

  @Override
  public MassUnit unit() {
    return unit;
  }
}
