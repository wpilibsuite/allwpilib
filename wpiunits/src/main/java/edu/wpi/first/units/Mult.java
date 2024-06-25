// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Mult<A extends Unit, B extends Unit> extends MeasureBase<MultUnit<A, B>> {
  public Mult(double magnitude, double baseUnitMagnitude, MultUnit<A, B> unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Measure<MultUnit<A, B>> copy() {
    return this;
  }
}
