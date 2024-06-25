// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Second;
import static edu.wpi.first.units.Units.Volts;

public class Voltage implements Measure<VoltageUnit> {
  private static final MathHelper<VoltageUnit, Voltage> mathHelper = new MathHelper<>(Volts::of);

  protected double magnitude;
  protected double baseUnitMagnitude;
  protected VoltageUnit unit;

  public Voltage(double magnitude, double baseUnitMagnitude, VoltageUnit unit) {
    this.magnitude = magnitude;
    this.baseUnitMagnitude = baseUnitMagnitude;
    this.unit = unit;
  }

  @Override
  public Voltage copy() {
    return this;
  }

  public Velocity<VoltageUnit> divide(Time period) {
    return mathHelper.divide(this, period, Volts.per(Second)::of);
  }

  public Velocity<VoltageUnit> per(TimeUnit period) {
    return divide(period.of(1));
  }

  @Override
  public double magnitude() {
    return magnitude;
  }

  @Override
  public double baseUnitMagnitude() {
    return baseUnitMagnitude;
  }

  @Override
  public VoltageUnit unit() {
    return unit;
  }

  public <U extends Unit> Measure<U> divide(Measure<? extends PerUnit<VoltageUnit, U>> m) {
    return ImmutableMeasure.ofBaseUnits(
        baseUnitMagnitude / m.baseUnitMagnitude(), m.unit().denominator());
  }
}
