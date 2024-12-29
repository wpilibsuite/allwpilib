// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.ImmutableMeasure;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.measure.Dimensionless;

/**
 * A generic implementation of a mutable measure. This is used in scenarios no unit-specific mutable
 * implementation can be determined.
 *
 * @param <U> the unit of measure
 */
public final class GenericMutableMeasureImpl<U extends Unit>
    extends MutableMeasureBase<U, Measure<U>, GenericMutableMeasureImpl<U>> {
  /**
   * Initializes the mutable measure with initial conditions. Both relative and base unit magnitudes
   * are required to avoid unnecessary calculations. It is up to the caller to ensure they are
   * correct.
   *
   * @param initialValue the initial magnitude of the measure, in terms of the unit
   * @param baseUnitMagnitude the initial magnitude of the measure, in terms of the base unit
   * @param unit the initial unit of measure
   */
  public GenericMutableMeasureImpl(double initialValue, double baseUnitMagnitude, U unit) {
    super(initialValue, baseUnitMagnitude, unit);
  }

  @Override
  public Measure<U> copy() {
    return ImmutableMeasure.ofBaseUnits(m_baseUnitMagnitude, m_unit);
  }

  @Override
  public MutableMeasure<U, ?, ?> mutableCopy() {
    return new GenericMutableMeasureImpl<>(m_magnitude, m_baseUnitMagnitude, m_unit);
  }

  @Override
  public Measure<U> unaryMinus() {
    return ImmutableMeasure.ofBaseUnits(0 - m_baseUnitMagnitude, m_unit);
  }

  @Override
  public Measure<U> plus(Measure<? extends U> other) {
    return ImmutableMeasure.ofBaseUnits(m_baseUnitMagnitude + other.baseUnitMagnitude(), m_unit);
  }

  @Override
  public Measure<U> minus(Measure<? extends U> other) {
    return ImmutableMeasure.ofBaseUnits(m_baseUnitMagnitude - other.baseUnitMagnitude(), m_unit);
  }

  @Override
  public Measure<U> times(double multiplier) {
    return ImmutableMeasure.ofBaseUnits(m_baseUnitMagnitude * multiplier, m_unit);
  }

  @Override
  public Measure<U> times(Dimensionless multiplier) {
    return ImmutableMeasure.ofBaseUnits(
        m_baseUnitMagnitude * multiplier.baseUnitMagnitude(), m_unit);
  }

  @Override
  public Measure<U> div(double divisor) {
    return ImmutableMeasure.ofBaseUnits(m_baseUnitMagnitude / divisor, m_unit);
  }

  @Override
  public Measure<U> div(Dimensionless divisor) {
    return ImmutableMeasure.ofBaseUnits(m_baseUnitMagnitude / divisor.baseUnitMagnitude(), m_unit);
  }
}
