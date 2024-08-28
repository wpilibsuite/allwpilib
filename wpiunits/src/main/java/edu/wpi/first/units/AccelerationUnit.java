// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.Acceleration;
import edu.wpi.first.units.measure.ImmutableAcceleration;
import edu.wpi.first.units.measure.MutAcceleration;

/**
 * A generic unit of acceleration.
 *
 * <p><strong>NOTE:</strong> This type is not compatible with unit-specific accelerations like
 * {@link edu.wpi.first.units.measure.LinearAcceleration}. Authors of APIs that need to interact
 * with all types should consider using a generic {@code Measure<? extends PerUnit<? extends
 * PerUnit<[dimension>], TimeUnit>, TimeUnit>}. Bounded wildcards are necessary in order to
 * interoperate with <i>any</i> subclass of the {@link edu.wpi.first.units.measure.Per} measurement
 * type.
 *
 * @param <D> the unit of the accelerating quantity
 */
public final class AccelerationUnit<D extends Unit> extends PerUnit<VelocityUnit<D>, TimeUnit> {
  @SuppressWarnings({"rawtypes", "unchecked"})
  private static final CombinatoryUnitCache<VelocityUnit, TimeUnit, AccelerationUnit> cache =
      new CombinatoryUnitCache<>(AccelerationUnit::new);

  AccelerationUnit(VelocityUnit<D> velocity, TimeUnit period) {
    super(
        velocity.isBaseUnit() && period.isBaseUnit()
            ? null
            : combine(velocity.getBaseUnit(), period.getBaseUnit()),
        velocity,
        period);
  }

  AccelerationUnit(
      AccelerationUnit<D> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  @Override
  public Acceleration<D> of(double magnitude) {
    return new ImmutableAcceleration<>(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Acceleration<D> ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableAcceleration<>(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  @SuppressWarnings({"unchecked", "rawtypes"})
  public Acceleration<D> zero() {
    return (Acceleration<D>) (Acceleration) super.zero();
  }

  @Override
  @SuppressWarnings({"unchecked", "rawtypes"})
  public Acceleration<D> one() {
    return (Acceleration<D>) (Acceleration) super.one();
  }

  @Override
  public MutAcceleration<D> mutable(double initialMagnitude) {
    return new MutAcceleration<>(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  @Override
  public VelocityUnit<AccelerationUnit<D>> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<AccelerationUnit<D>, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another time unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other time unit
   * @param otherUnit the other time unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, AccelerationUnit<D> otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  /**
   * Combines a generic velocity and time period into a unit of acceleration.
   *
   * @param velocity the unit of velocity
   * @param period the unit of the time period of acceleration
   * @param <D> the unit of the accelerating quantity
   * @return the combined acceleration unit
   */
  @SuppressWarnings("unchecked")
  public static <D extends Unit> AccelerationUnit<D> combine(
      VelocityUnit<D> velocity, TimeUnit period) {
    return cache.combine(velocity, period);
  }
}
