// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.AccelerationUnit;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.immutable.ImmutableAcceleration;
import edu.wpi.first.units.measure.Acceleration;

public final class MutAcceleration<D extends Unit>
    extends MutableMeasureBase<AccelerationUnit<D>, Acceleration<D>, MutAcceleration<D>>
    implements Acceleration<D> {
  private final MathHelper<AccelerationUnit<D>, Acceleration<D>> mathHelper =
      new MathHelper<>(baseUnit()::of);

  public MutAcceleration(double magnitude, double baseUnitMagnitude, AccelerationUnit<D> unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MathHelper<AccelerationUnit<D>, Acceleration<D>> getMathHelper() {
    return mathHelper;
  }

  @Override
  public Acceleration<D> copy() {
    return new ImmutableAcceleration<>(magnitude(), baseUnitMagnitude(), unit());
  }
}
