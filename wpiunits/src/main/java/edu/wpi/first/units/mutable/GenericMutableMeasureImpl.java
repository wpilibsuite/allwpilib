// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.ImmutableMeasure;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.measure.Dimensionless;

public final class GenericMutableMeasureImpl<U extends Unit>
    extends MutableMeasureBase<U, Measure<U>, GenericMutableMeasureImpl<U>> {
  public GenericMutableMeasureImpl(double initialValue, double baseUnitMagnitude, U unit) {
    super(initialValue, baseUnitMagnitude, unit);
  }

  public GenericMutableMeasureImpl(double initialValue, U unit) {
    this(initialValue, unit.toBaseUnits(initialValue), unit);
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
  public Measure<U> divide(double divisor) {
    return ImmutableMeasure.ofBaseUnits(m_baseUnitMagnitude / divisor, m_unit);
  }

  @Override
  public Measure<U> divide(Dimensionless divisor) {
    return ImmutableMeasure.ofBaseUnits(m_baseUnitMagnitude / divisor.baseUnitMagnitude(), m_unit);
  }
}
