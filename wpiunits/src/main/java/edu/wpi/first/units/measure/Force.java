// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.Joules;
import static edu.wpi.first.units.Units.Newtons;
import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.DistanceUnit;
import edu.wpi.first.units.ForceUnit;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.TimeUnit;
import edu.wpi.first.units.VelocityUnit;

public interface Force extends Measure<ForceUnit> {
  MathHelper<ForceUnit, Force> mathHelper = new MathHelper<>(Newtons::of);

  @Override
  Force copy();

  default Force plus(Force other) {
    return mathHelper.add(this, other);
  }

  default Force minus(Force other) {
    return mathHelper.minus(this, other);
  }

  default Dimensionless divide(Force other) {
    return mathHelper.divide(this, other);
  }

  default Velocity<ForceUnit> divide(Time time) {
    return mathHelper.divide(this, time, VelocityUnit.combine(baseUnit(), Seconds)::of);
  }

  default Velocity<ForceUnit> per(TimeUnit time) {
    return divide(time.of(1));
  }

  default Energy mult(Distance distance) {
    return mathHelper.multiply(this, distance, Joules::of);
  }

  default Energy mult(DistanceUnit distance) {
    return mult(distance.of(1));
  }
}
