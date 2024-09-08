// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Value;

import edu.wpi.first.units.measure.Frequency;
import edu.wpi.first.units.measure.ImmutableFrequency;
import edu.wpi.first.units.measure.MutFrequency;

/** A unit of frequency like {@link edu.wpi.first.units.Units#Hertz}. */
public final class FrequencyUnit extends PerUnit<DimensionlessUnit, TimeUnit> {
  private static final CombinatoryUnitCache<DimensionlessUnit, TimeUnit, FrequencyUnit> cache =
      new CombinatoryUnitCache<>(FrequencyUnit::new);

  FrequencyUnit(DimensionlessUnit numerator, TimeUnit denominator) {
    super(
        numerator.isBaseUnit() && denominator.isBaseUnit()
            ? null
            : combine(numerator.getBaseUnit(), denominator.getBaseUnit()),
        numerator,
        denominator);
  }

  FrequencyUnit(
      FrequencyUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a dimensionless unit and a cycle period to create a frequency.
   *
   * @param dim the dimensionless unit
   * @param period the unit of time
   * @return the combined unit of frequency
   */
  public static FrequencyUnit combine(DimensionlessUnit dim, TimeUnit period) {
    return cache.combine(dim, period);
  }

  @Override
  public FrequencyUnit getBaseUnit() {
    return (FrequencyUnit) super.getBaseUnit();
  }

  /**
   * Inverts a unit of time to get its corresponding frequency (as if the unit of time is the period
   * of the frequency).
   *
   * @param time period of the associated frequency
   * @return the frequency associated with the period
   */
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
  public Frequency zero() {
    return (Frequency) super.zero();
  }

  @Override
  public Frequency one() {
    return (Frequency) super.one();
  }

  @Override
  public MutFrequency mutable(double initialMagnitude) {
    return new MutFrequency(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  @Override
  public VelocityUnit<FrequencyUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<FrequencyUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, FrequencyUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
