// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public interface Angle extends Measure<AngleUnit> {
  Dimensionless divide(Angle other);

  Angle plus(Angle other);

  Angle minus(Angle other);

  Angle divide(double divisor);

  AngularVelocity divide(Time period);

  <Other extends Unit> Measure<Other> divide(
      Measure<? extends PerUnit<AngleUnit, ? extends Other>> ratio);

  AngularVelocity per(TimeUnit unit);

  Dimensionless per(AngleUnit unit);

  <Divisor extends Unit> Measure<? extends PerUnit<AngleUnit, Divisor>> per(Divisor divisor);

  <Other extends Unit> Measure<Other> times(
      Measure<? extends PerUnit<? extends Other, AngleUnit>> ratio);
}
