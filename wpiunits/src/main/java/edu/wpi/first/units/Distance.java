// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.Seconds;

public interface Distance extends Measure<DistanceUnit> {
  MathHelper<DistanceUnit, Distance> mathHelper = new MathHelper<>(Meters::of);

  @Override
  Distance copy();

  default LinearVelocity per(Time period) {
    return new ImmutableLinearVelocity(
        magnitude() / period.magnitude(),
        baseUnitMagnitude() / period.baseUnitMagnitude(),
        this.unit().per(period.unit()));
  }

  default LinearVelocity per(TimeUnit period) {
    return new ImmutableLinearVelocity(
        magnitude() / period.fromBaseUnits(1),
        baseUnitMagnitude() / period.toBaseUnits(1),
        this.unit().per(period));
  }

  default Distance plus(Distance other) {
    return mathHelper.add(this, other);
  }

  default Distance minus(Distance other) {
    return mathHelper.minus(this, other);
  }

  default Distance times(double scalar) {
    return mathHelper.multiply(this, scalar);
  }

  default <U extends Unit> Measure<U> times(Measure<? extends PerUnit<U, DistanceUnit>> term) {
    return mathHelper.multiply(
        this,
        term,
        mag -> {
          var u = term.baseUnit().numerator();
          return ImmutableMeasure.ofBaseUnits(mag, u);
        });
  }

  default LinearVelocity times(Frequency frequency) {
    return mathHelper.multiply(this, frequency, MetersPerSecond::of);
  }

  default Distance divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  default Distance negate() {
    return mathHelper.negate(this);
  }

  default Distance divide(Dimensionless divisor) {
    return mathHelper.divide(this, divisor);
  }

  default Time divide(LinearVelocity velocity) {
    return mathHelper.divide(this, velocity, Seconds::of);
  }
}
