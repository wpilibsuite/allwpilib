// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class AccelerationUnit<D extends Unit> extends Per<VelocityUnit<D>, TimeUnit> {
  @SuppressWarnings({"rawtypes", "unchecked"})
  private static final CombinatoryUnitCache<VelocityUnit, TimeUnit, AccelerationUnit> cache =
      new CombinatoryUnitCache<>(AccelerationUnit::new);

  protected AccelerationUnit(VelocityUnit<D> velocity, TimeUnit period) {
    super(velocity, period);
  }

  protected AccelerationUnit(
      AccelerationUnit<D> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public Acceleration<D> of(double magnitude) {
    return new Acceleration<>(magnitude, toBaseUnits(magnitude), this);
  }

  @SuppressWarnings("unchecked")
  public static <D extends Unit> AccelerationUnit<D> combine(
      VelocityUnit<D> velocity, TimeUnit period) {
    return cache.combine(velocity, period);
  }
}
