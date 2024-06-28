// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Value;

import edu.wpi.first.units.measure.Frequency;
import edu.wpi.first.units.measure.ImmutableFrequency;
import edu.wpi.first.units.measure.MutFrequency;

public class FrequencyUnit extends PerUnit<DimensionlessUnit, TimeUnit> {
  private static final CombinatoryUnitCache<DimensionlessUnit, TimeUnit, FrequencyUnit> cache =
      new CombinatoryUnitCache<>(FrequencyUnit::new);

  protected FrequencyUnit(DimensionlessUnit numerator, TimeUnit denominator) {
    super(numerator, denominator);
  }

  FrequencyUnit(
      FrequencyUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public static FrequencyUnit combine(DimensionlessUnit dim, TimeUnit time) {
    return cache.combine(dim, time);
  }

  public static FrequencyUnit inverse(TimeUnit time) {
    return combine(Value, time);
  }

  @Override
  public Frequency of(double magnitude) {
    return new ImmutableFrequency(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Frequency ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableFrequency(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public MutFrequency mutable(double initialMagnitude) {
    return new MutFrequency(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }
}
