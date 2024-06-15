// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public record Dimensionless(double magnitude, double baseUnitMagnitude, DimensionlessUnit unit)
    implements Measure<DimensionlessUnit> {
  @Override
  public Dimensionless copy() {
    return this;
  }

  public <U extends Unit> Measure<U> mult(Measure<U> other) {
    // TODO
    return other;
  }
}
