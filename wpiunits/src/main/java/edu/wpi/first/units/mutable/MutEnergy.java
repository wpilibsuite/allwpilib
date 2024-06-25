// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Energy;
import edu.wpi.first.units.EnergyUnit;
import edu.wpi.first.units.MutableMeasure;

public class MutEnergy extends Energy implements MutableMeasure<EnergyUnit, Energy, MutEnergy> {
  public MutEnergy(double magnitude, double baseUnitMagnitude, EnergyUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutEnergy mut_replace(double magnitude, EnergyUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public Energy copy() {
    return new Energy(magnitude, baseUnitMagnitude, unit);
  }
}
