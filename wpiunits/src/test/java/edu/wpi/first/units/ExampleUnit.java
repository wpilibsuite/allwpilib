// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

class ExampleUnit extends Unit {
  ExampleUnit(double baseUnitEquivalent) {
    this(baseUnitEquivalent, "Example", "ex");
  }

  ExampleUnit(
      ExampleUnit baseUnit,
      UnaryFunction toBase,
      UnaryFunction fromBase,
      String name,
      String symbol) {
    super(baseUnit, toBase, fromBase, name, symbol);
  }

  ExampleUnit(double baseUnitEquivalent, String name, String symbol) {
    super(null, baseUnitEquivalent, name, symbol);
  }

  public double convertFrom(double magnitude, ExampleUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  @Override
  public Measure<ExampleUnit> of(double magnitude) {
    return ImmutableMeasure.ofRelativeUnits(magnitude, this);
  }

  @Override
  public Measure<ExampleUnit> ofBaseUnits(double baseUnitMagnitude) {
    return ImmutableMeasure.ofBaseUnits(baseUnitMagnitude, this);
  }

  @Override
  public VelocityUnit<ExampleUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }
}
