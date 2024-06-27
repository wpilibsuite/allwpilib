// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.PerUnit;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.immutable.ImmutableRatio;
import edu.wpi.first.units.measure.Ratio;

public final class MutRatio<N extends Unit, D extends Unit>
    extends MutableMeasureBase<PerUnit<N, D>, Ratio<N, D>, MutRatio<N, D>> implements Ratio<N, D> {
  public MutRatio(double magnitude, double baseUnitMagnitude, PerUnit<N, D> unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Ratio<N, D> copy() {
    return new ImmutableRatio<>(magnitude(), baseUnitMagnitude(), unit());
  }
}
