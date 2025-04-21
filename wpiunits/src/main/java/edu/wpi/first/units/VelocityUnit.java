// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableVelocity;
import edu.wpi.first.units.measure.MutVelocity;
import edu.wpi.first.units.measure.Velocity;

/**
 * Unit of velocity dimension that is a combination of a distance unit (numerator) and a time unit
 * (denominator).
 *
 * <p>This is a generic velocity type for units that do not have discrete velocity units (eg {@link
 * DistanceUnit} has {@link LinearVelocityUnit}, and so would not use this class).
 *
 * @param <D> the unit of the changing quantity, such as {@link VoltageUnit} or {@link
 *     TemperatureUnit}
 */
public final class VelocityUnit<D extends Unit> extends PerUnit<D, TimeUnit> {
  @SuppressWarnings("rawtypes")
  private static final CombinatoryUnitCache<Unit, TimeUnit, VelocityUnit> cache =
      new CombinatoryUnitCache<>(VelocityUnit::new);

  @SuppressWarnings("unchecked")
  VelocityUnit(D numerator, TimeUnit denominator) {
    super(
        numerator.isBaseUnit() && denominator.isBaseUnit()
            ? null
            : combine((D) numerator.getBaseUnit(), denominator.getBaseUnit()),
        numerator,
        denominator);
  }

  VelocityUnit(
      VelocityUnit<D> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a dimension unit and a unit of the period of change to create a unit of velocity.
   *
   * @param unit the unit of the changing dimension
   * @param period the unit of the period of change in the velocity
   * @param <D> the unit of the changing dimension
   * @return the combined velocity unit
   */
  @SuppressWarnings("unchecked")
  public static <D extends Unit> VelocityUnit<D> combine(D unit, TimeUnit period) {
    return cache.combine(unit, period);
  }

  @Override
  public VelocityUnit<D> getBaseUnit() {
    return (VelocityUnit<D>) super.getBaseUnit();
  }

  /**
   * Gets the major unit being measured (eg Meters for Meters per Second).
   *
   * @return the major unit
   */
  public D getUnit() {
    return numerator();
  }

  /**
   * Gets the period unit of the velocity, eg Seconds or Milliseconds.
   *
   * @return the period unit
   */
  public TimeUnit getPeriod() {
    return denominator();
  }

  @Override
  public Velocity<D> of(double magnitude) {
    return new ImmutableVelocity<>(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Velocity<D> ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableVelocity<>(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  @SuppressWarnings("unchecked")
  public Velocity<D> zero() {
    return (Velocity<D>) super.zero();
  }

  @Override
  @SuppressWarnings("unchecked")
  public Velocity<D> one() {
    return (Velocity<D>) super.one();
  }

  @Override
  public MutableMeasure<VelocityUnit<D>, ?, ?> mutable(double initialMagnitude) {
    return new MutVelocity<>(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  /**
   * Combines this velocity unit with a unit of a period of change to create an acceleration unit.
   *
   * @param period the unit of the period of change
   * @return the acceleration unit
   */
  @Override
  public AccelerationUnit<D> per(TimeUnit period) {
    return AccelerationUnit.combine(this, period);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<VelocityUnit<D>, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another velocity unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other velocity unit
   * @param otherUnit the other velocity unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, VelocityUnit<? extends D> otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
