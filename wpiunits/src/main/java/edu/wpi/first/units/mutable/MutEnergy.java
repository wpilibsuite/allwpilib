// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.EnergyUnit;
import edu.wpi.first.units.immutable.ImmutableEnergy;
import edu.wpi.first.units.measure.Energy;

public final class MutEnergy extends MutableMeasureBase<EnergyUnit, Energy, MutEnergy>
    implements Energy {
  public MutEnergy(double magnitude, double baseUnitMagnitude, EnergyUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Energy copy() {
    return new ImmutableEnergy(magnitude, baseUnitMagnitude, unit);
  }
}
