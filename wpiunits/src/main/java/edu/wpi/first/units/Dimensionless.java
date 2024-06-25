// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Hertz;
import static edu.wpi.first.units.Units.Value;

public class Dimensionless extends MeasureBase<DimensionlessUnit> {
  private static final MathHelper<DimensionlessUnit, Dimensionless> mathHelper =
      new MathHelper<>(Value::of);

  public Dimensionless(double magnitude, double baseUnitMagnitude, DimensionlessUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Dimensionless copy() {
    return this;
  }

  public <U extends Unit> Measure<U> times(Measure<U> other) {
    return (Measure<U>)
        other.unit().ofBaseUnits(other.baseUnitMagnitude() * this.baseUnitMagnitude);
  }

  public Frequency divide(Time time) {
    return Hertz.of(baseUnitMagnitude / time.baseUnitMagnitude());
  }
}
