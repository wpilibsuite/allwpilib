// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public interface Acceleration<D extends Unit> extends Measure<AccelerationUnit<D>> {
  @Override
  Acceleration<D> copy();

  Acceleration<D> plus(Acceleration<D> other);

  Acceleration<D> minus(Acceleration<D> other);

  Dimensionless divide(Acceleration<D> divisor);

  Acceleration<D> divide(double divisor);

  Acceleration<D> divide(Dimensionless divisor);

  Acceleration<D> times(double multiplier);

  Acceleration<D> times(Dimensionless multiplier);

  Velocity<D> times(Time time);
}
