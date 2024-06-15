// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import java.util.Objects;

/**
 * Generic combinatory unit type that represents the proportion of one unit to another, such as
 * Meters per Second or Radians per Celsius.
 *
 * <p>Note: {@link VelocityUnit} is used to represent the velocity dimension, rather than {@code
 * Per<DistanceUnit, TimeUnit>}.
 *
 * @param <N> the type of the numerator unit
 * @param <D> the type of the denominator unit
 */
public class Per<N extends Unit, D extends Unit> extends Unit {
  private final N m_numerator;
  private final D m_denominator;

  /**
   * Keep a cache of created instances so expressions like Volts.per(Meter) don't do any allocations
   * after the first.
   */
  @SuppressWarnings("rawtypes")
  private static final CombinatoryUnitCache<Unit, Unit, Per> cache =
      new CombinatoryUnitCache<>(Per::new);

  /**
   * Creates a new proportional unit derived from the ratio of one unit to another. Consider using
   * {@link #combine} instead of manually calling this constructor.
   *
   * @param numerator the numerator unit
   * @param denominator the denominator unit
   */
  protected Per(N numerator, D denominator) {
    super(
        numerator.isBaseUnit() && denominator.isBaseUnit()
            ? null
            : combine(numerator.getBaseUnit(), denominator.getBaseUnit()),
        numerator.toBaseUnits(1) / denominator.toBaseUnits(1),
        numerator.name() + " per " + denominator.name(),
        numerator.symbol() + "/" + denominator.symbol());
    m_numerator = numerator;
    m_denominator = denominator;
  }

  Per(
      Per<N, D> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_numerator = getBaseUnit().numerator();
    m_denominator = getBaseUnit().denominator();
  }

  /**
   * Creates a new Per unit derived from an arbitrary numerator and time denominator units. Using a
   * denominator with a unit of time is discouraged; use {@link VelocityUnit} instead.
   *
   * <pre>
   *   Per.combine(Volts, Meters) // possible PID constant
   * </pre>
   *
   * @param <N> the type of the numerator unit
   * @param <D> the type of the denominator unit
   * @param numerator the numerator unit
   * @param denominator the denominator for unit time
   * @return the combined unit
   */
  @SuppressWarnings("unchecked")
  public static <N extends Unit, D extends Unit> Per<N, D> combine(N numerator, D denominator) {
    return cache.combine(numerator, denominator);
  }

  @Override
  @SuppressWarnings("unchecked")
  public Per<N, D> getBaseUnit() {
    return (Per<N, D>) super.getBaseUnit();
  }

  /**
   * Gets the numerator unit. For a {@code Per<A, B>}, this will return the {@code A} unit.
   *
   * @return the numerator unit
   */
  public N numerator() {
    return m_numerator;
  }

  /**
   * Gets the denominator unit. For a {@code Per<A, B>}, this will return the {@code B} unit.
   *
   * @return the denominator unit
   */
  public D denominator() {
    return m_denominator;
  }

  /**
   * Returns the reciprocal of this Per.
   *
   * @return the reciprocal
   */
  public Per<D, N> reciprocal() {
    return combine(m_denominator, m_numerator);
  }

  public N mult(D denom) {
    if (denom.equivalent(denominator())) {
      return numerator();
    }

    return Units.derive(numerator())
        .toBase(denom.getConverterToBase().div(denominator().getConverterToBase()))
        .fromBase(denom.getConverterFromBase().div(denominator().getConverterFromBase()))
        .named(name() + " per " + denom.name())
        .symbol(symbol() + "/" + denom.symbol())
        .make();
  }

  public Measure<? extends Per<N, D>> of(double magnitude) {
    return ImmutableMeasure.ofRelativeUnits(magnitude, this);
  }

  public double convertFrom(double magnitude, Per<? extends N, ? extends D> otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }
    if (!super.equals(o)) {
      return false;
    }
    Per<?, ?> per = (Per<?, ?>) o;
    return Objects.equals(m_numerator, per.m_numerator)
        && Objects.equals(m_denominator, per.m_denominator);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_numerator, m_denominator);
  }
}
