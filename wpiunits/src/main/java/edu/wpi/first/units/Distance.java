// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.Seconds;

public class Distance extends MeasureBase<DistanceUnit> {
  private static final MathHelper<DistanceUnit, Distance> mathHelper = new MathHelper<>(Meters::of);

  public Distance(double magnitude, double baseUnitMagnitude, DistanceUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Distance copy() {
    return this;
  }

  public LinearVelocity per(Time period) {
    return new LinearVelocity(
        magnitude / period.magnitude(),
        baseUnitMagnitude / period.baseUnitMagnitude(),
        this.unit.per(period.unit()));
  }

  public LinearVelocity per(TimeUnit period) {
    return new LinearVelocity(
        magnitude / period.fromBaseUnits(1),
        baseUnitMagnitude / period.toBaseUnits(1),
        this.unit.per(period));
  }

  @Override
  public DistanceUnit unit() {
    return unit;
  }

  public Distance plus(Distance other) {
    return mathHelper.add(this, other);
  }

  public Distance minus(Distance other) {
    return mathHelper.minus(this, other);
  }

  public Distance times(double scalar) {
    return mathHelper.multiply(this, scalar);
  }

  public <U extends Unit> Measure<U> times(Measure<? extends PerUnit<U, DistanceUnit>> term) {
    return mathHelper.multiply(
        this,
        term,
        mag -> {
          var u = term.baseUnit().numerator();
          return ImmutableMeasure.ofBaseUnits(mag, u);
        });
  }

  public LinearVelocity times(Frequency frequency) {
    return mathHelper.multiply(this, frequency, MetersPerSecond::of);
  }

  public Distance divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  public Distance negate() {
    return mathHelper.negate(this);
  }

  public Distance divide(Dimensionless divisor) {
    return mathHelper.divide(this, divisor);
  }

  public Time divide(LinearVelocity velocity) {
    return mathHelper.divide(this, velocity, Seconds::of);
  }
}
