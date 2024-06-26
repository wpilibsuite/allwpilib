// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.immutable;

import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.Value;

import edu.wpi.first.units.Angle;
import edu.wpi.first.units.AngleUnit;
import edu.wpi.first.units.AngularVelocity;
import edu.wpi.first.units.Dimensionless;
import edu.wpi.first.units.ImmutableMeasure;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.PerUnit;
import edu.wpi.first.units.Time;
import edu.wpi.first.units.TimeUnit;
import edu.wpi.first.units.Unit;

public record ImmutableAngle(double magnitude, double baseUnitMagnitude, AngleUnit unit)
    implements Angle {
  private static final MathHelper<AngleUnit, Angle> mathHelper = new MathHelper<>(Radians::of);

  @Override
  public Angle copy() {
    return this;
  }

  @Override
  public Dimensionless divide(Angle other) {
    return mathHelper.divide(this, other);
  }

  @Override
  public Angle plus(Angle other) {
    return mathHelper.add(this, other);
  }

  @Override
  public Angle minus(Angle other) {
    return mathHelper.minus(this, other);
  }

  @Override
  public Angle divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  @Override
  public AngularVelocity divide(Time period) {
    return mathHelper.divide(this, period, RadiansPerSecond::of);
  }

  @Override
  public <Other extends Unit> Measure<Other> divide(
      Measure<? extends PerUnit<AngleUnit, ? extends Other>> ratio) {
    return ImmutableMeasure.ofBaseUnits(
        baseUnitMagnitude / ratio.baseUnitMagnitude(), ratio.unit().denominator());
  }

  @Override
  public AngularVelocity per(TimeUnit unit) {
    return mathHelper.divide(this, unit.of(1), RadiansPerSecond::of);
  }

  @Override
  public Dimensionless per(AngleUnit unit) {
    return mathHelper.divide(this, unit.of(1), Value::of);
  }

  @Override
  @SuppressWarnings("unchecked")
  public <Divisor extends Unit> Measure<? extends PerUnit<AngleUnit, Divisor>> per(
      Divisor divisor) {
    return PerUnit.combine(this.baseUnit(), (Divisor) divisor.getBaseUnit())
        .of(baseUnitMagnitude / divisor.fromBaseUnits(1));
  }

  @Override
  public <Other extends Unit> Measure<Other> times(
      Measure<? extends PerUnit<? extends Other, AngleUnit>> ratio) {
    return ImmutableMeasure.ofBaseUnits(
        baseUnitMagnitude * ratio.baseUnitMagnitude(), ratio.unit().numerator());
  }
}
