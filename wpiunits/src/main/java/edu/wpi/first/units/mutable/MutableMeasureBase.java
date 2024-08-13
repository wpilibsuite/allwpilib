// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Unit;
import java.util.Objects;

/**
 * A partial implementation of {@link MutableMeasure} to handle most of the state and Java object-
 * related implementations.
 *
 * @param <U> The dimension of measurement.
 * @param <Base> The base measure type.
 * @param <MutSelf> The self type. This MUST inherit from the base measure type.
 */
public abstract class MutableMeasureBase<
        U extends Unit, Base extends Measure<U>, MutSelf extends MutableMeasure<U, Base, MutSelf>>
    implements Measure<U>, MutableMeasure<U, Base, MutSelf> {
  /** The magnitude of the measurement, in terms of {@link #m_unit}. */
  protected double m_magnitude;

  /** The magnitude of the measurement, in terms of {@link #m_unit}'s base unit. */
  protected double m_baseUnitMagnitude;

  /** The unit of measurement. */
  protected U m_unit;

  /**
   * Initializes the mutable measure with initial conditions. Both relative and base unit magnitudes
   * are required to avoid unnecessary calculations. It is up to the caller to ensure they are
   * correct.
   *
   * @param magnitude the initial magnitude of the measure, in terms of the unit
   * @param baseUnitMagnitude the initial magnitude of the measure, in terms of the base unit
   * @param unit the initial unit of measure
   */
  protected MutableMeasureBase(double magnitude, double baseUnitMagnitude, U unit) {
    this.m_magnitude = magnitude;
    this.m_baseUnitMagnitude = baseUnitMagnitude;
    this.m_unit = Objects.requireNonNull(unit, "Unit cannot be null");
  }

  @Override
  public double magnitude() {
    return m_magnitude;
  }

  @Override
  public double baseUnitMagnitude() {
    return m_baseUnitMagnitude;
  }

  @Override
  public U unit() {
    return m_unit;
  }

  @Override
  public String toString() {
    return toShortString();
  }

  @Override
  public boolean equals(Object o) {
    return this == o || o instanceof Measure<?> m && isEquivalent(m);
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_magnitude, m_baseUnitMagnitude, m_unit);
  }

  @Override
  @SuppressWarnings("unchecked")
  public MutSelf mut_replace(double magnitude, U newUnit) {
    this.m_unit = Objects.requireNonNull(newUnit, "New unit cannot be null");
    this.m_magnitude = magnitude;
    this.m_baseUnitMagnitude = m_unit.toBaseUnits(magnitude);

    return (MutSelf) this;
  }
}
