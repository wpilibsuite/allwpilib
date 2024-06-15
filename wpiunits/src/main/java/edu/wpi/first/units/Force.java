// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Joules;
import static edu.wpi.first.units.Units.Newtons;
import static edu.wpi.first.units.Units.Seconds;

public record Force(double magnitude, double baseUnitMagnitude, ForceUnit unit)
    implements Measure<ForceUnit> {
  private static final MathHelper<ForceUnit, Force> mathHelper = new MathHelper<>(Newtons::of);

  @Override
  public Force copy() {
    return this;
  }

  public Force plus(Force other) {
    return mathHelper.add(this, other);
  }

  public Force minus(Force other) {
    return mathHelper.minus(this, other);
  }

  public Dimensionless divide(Force other) {
    return mathHelper.divide(this, other);
  }

  public Velocity<ForceUnit> divide(Time time) {
    return mathHelper.divide(this, time, VelocityUnit.combine(baseUnit(), Seconds)::of);
  }

  public Velocity<ForceUnit> per(TimeUnit time) {
    return divide(time.of(1));
  }

  public Energy mult(Distance distance) {
    return mathHelper.multiply(this, distance, Joules::of);
  }

  public Energy mult(DistanceUnit distance) {
    return mult(distance.of(1));
  }
}
