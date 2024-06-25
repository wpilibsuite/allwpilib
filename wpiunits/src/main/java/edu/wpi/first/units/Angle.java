// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.Value;

public class Angle extends MeasureBase<AngleUnit> {
  private static final MathHelper<AngleUnit, Angle> mathHelper = new MathHelper<>(Radians::of);

  public Angle(double magnitude, double baseUnitMagnitude, AngleUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Angle copy() {
    return this;
  }

  public Dimensionless divide(Angle other) {
    return mathHelper.divide(this, other);
  }

  public Angle plus(Angle other) {
    return mathHelper.add(this, other);
  }

  public Angle minus(Angle other) {
    return mathHelper.minus(this, other);
  }

  public Angle divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  public AngularVelocity divide(Time period) {
    return mathHelper.divide(this, period, RadiansPerSecond::of);
  }

  public <Other extends Unit> Measure<Other> divide(
      Measure<? extends PerUnit<AngleUnit, ? extends Other>> ratio) {
    return ImmutableMeasure.ofBaseUnits(
        baseUnitMagnitude / ratio.baseUnitMagnitude(), ratio.unit().denominator());
  }

  public AngularVelocity per(TimeUnit unit) {
    return mathHelper.divide(this, unit.of(1), RadiansPerSecond::of);
  }

  public Dimensionless per(AngleUnit unit) {
    return mathHelper.divide(this, unit.of(1), Value::of);
  }

  @SuppressWarnings("unchecked")
  public <Divisor extends Unit> Measure<? extends PerUnit<AngleUnit, Divisor>> per(
      Divisor divisor) {
    return PerUnit.combine(this.baseUnit(), (Divisor) divisor.getBaseUnit())
        .of(baseUnitMagnitude / divisor.fromBaseUnits(1));
  }

  public <Other extends Unit> Measure<Other> times(
      Measure<? extends PerUnit<? extends Other, AngleUnit>> ratio) {
    return ImmutableMeasure.ofBaseUnits(
        baseUnitMagnitude * ratio.baseUnitMagnitude(), ratio.unit().numerator());
  }
}
