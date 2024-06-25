// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Joules;
import static edu.wpi.first.units.Units.Watts;

public class Energy extends MeasureBase<EnergyUnit> {
  private static final MathHelper<EnergyUnit, Energy> mathHelper = new MathHelper<>(Joules::of);

  public Energy(double magnitude, double baseUnitMagnitude, EnergyUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Energy copy() {
    return this;
  }

  public Energy plus(Energy other) {
    return mathHelper.add(this, other);
  }

  public Energy minus(Energy other) {
    return mathHelper.minus(this, other);
  }

  public Dimensionless divide(Energy other) {
    return mathHelper.divide(this, other);
  }

  public Power divide(Time period) {
    return mathHelper.divide(this, period, Watts::of);
  }
}
