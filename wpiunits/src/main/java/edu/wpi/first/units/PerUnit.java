// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutablePer;
import edu.wpi.first.units.measure.MutPer;
import edu.wpi.first.units.measure.Per;
import java.util.Objects;

/**
 * Generic combinatory unit type that represents the proportion of one unit to another, such as
 * Meters per Second or Radians per Celsius.
 *
 * @param <N> the type of the numerator unit
 * @param <D> the type of the denominator unit
 */
public class PerUnit<N extends Unit, D extends Unit> extends Unit {
  private final N m_numerator;
  private final D m_denominator;

  /**
   * Keep a cache of created instances so expressions like Volts.per(Meter) don't do any allocations
   * after the first.
   */
  @SuppressWarnings("rawtypes")
  private static final CombinatoryUnitCache<Unit, Unit, PerUnit> cache =
      new CombinatoryUnitCache<>(PerUnit::new);

  /**
   * Creates a new proportional unit derived from the ratio of one unit to another. Consider using
   * {@link #combine} instead of manually calling this constructor.
   *
   * @param numerator the numerator unit
   * @param denominator the denominator unit
   */
  private PerUnit(N numerator, D denominator) {
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

  /**
   * Creates a new proportional unit derived from the ratio of one unit to another. Subclasses of
   * {@code PerUnit} should use this constructor.
   *
   * @param baseUnit the base unit. Set this to null if the unit being constructed is its own base
   *     unit
   * @param numerator the numerator unit
   * @param denominator the denominator unit
   */
  protected PerUnit(PerUnit<N, D> baseUnit, N numerator, D denominator) {
    super(
        baseUnit,
        numerator.toBaseUnits(1) / denominator.toBaseUnits(1),
        numerator.name() + " per " + denominator.name(),
        numerator.symbol() + "/" + denominator.symbol());
    m_numerator = numerator;
    m_denominator = denominator;
  }

  /** {@inheritDoc} */
  PerUnit(
      PerUnit<N, D> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_numerator = getBaseUnit().numerator();
    m_denominator = getBaseUnit().denominator();
  }

  /**
   * Creates a new PerUnit unit derived from an arbitrary numerator and time denominator units.
   *
   * <pre>
   *   PerUnit.combine(Volts, Meters) // possible PID constant
   * </pre>
   *
   * @param <N> the type of the numerator unit
   * @param <D> the type of the denominator unit
   * @param numerator the numerator unit
   * @param denominator the denominator for unit time
   * @return the combined unit
   */
  @SuppressWarnings("unchecked")
  public static <N extends Unit, D extends Unit> PerUnit<N, D> combine(N numerator, D denominator) {
    return cache.combine(numerator, denominator);
  }

  @Override
  @SuppressWarnings("unchecked")
  public PerUnit<N, D> getBaseUnit() {
    return (PerUnit<N, D>) super.getBaseUnit();
  }

  /**
   * Gets the numerator unit. For a {@code PerUnit<A, B>}, this will return the {@code A} unit.
   *
   * @return the numerator unit
   */
  public N numerator() {
    return m_numerator;
  }

  /**
   * Gets the denominator unit. For a {@code PerUnit<A, B>}, this will return the {@code B} unit.
   *
   * @return the denominator unit
   */
  public D denominator() {
    return m_denominator;
  }

  /**
   * Returns the reciprocal of this PerUnit.
   *
   * @return the reciprocal
   */
  @SuppressWarnings("unchecked")
  public PerUnit<D, N> reciprocal() {
    if (m_numerator instanceof TimeUnit t) {
      // Dividing by time, return a velocity
      return (PerUnit<D, N>) VelocityUnit.combine(m_denominator, t);
    } else {
      // Generic case
      return combine(m_denominator, m_numerator);
    }
  }

  /**
   * Multiplies this unit by a unit of its denominator.
   *
   * @param denom the denominator-typed unit to multiply by
   * @return the result
   */
  public N mult(D denom) {
    if (denom.equivalent(denominator())) {
      return numerator();
    }

    return Units.derive(numerator())
        .toBase(denom.getConverterToBase().div(denominator().getConverterToBase()))
        .fromBase(denom.getConverterFromBase().div(denominator().getConverterFromBase()))
        .named(name() + " " + denom.name())
        .symbol(symbol() + "-" + denom.symbol())
        .make();
  }

  /**
   * {@inheritDoc}
   *
   * <p>Note: When called on an object of type {@code PerUnit} (and <i>not</i> a subclass!), this
   * method will always return a {@link edu.wpi.first.units.measure.Per} instance. If you want to
   * avoid casting, use {@link #ofNative(double)} that returns a {@code Per} instance directly.
   *
   * @param magnitude the magnitude of the measure
   * @return the ratio measure
   */
  @Override
  public Measure<? extends PerUnit<N, D>> of(double magnitude) {
    return ofNative(magnitude);
  }

  /**
   * {@inheritDoc}
   *
   * <p>Note: When called on an object of type {@code PerUnit} (and <i>not</i> a subclass!), this
   * method will always return a {@link edu.wpi.first.units.measure.Per} instance. If you want to
   * avoid casting, use {@link #ofNativeBaseUnits(double)} that returns a {@code Per} instance
   * directly.
   *
   * @param baseUnitMagnitude the magnitude of the measure in terms of its base units.
   * @return the ratio measure
   */
  @Override
  public Measure<? extends PerUnit<N, D>> ofBaseUnits(double baseUnitMagnitude) {
    return ofNativeBaseUnits(baseUnitMagnitude);
  }

  /**
   * Creates a new immutable measurement of the given magnitude in terms of the ratio unit. This
   * will always return a {@code Per} object and cannot be overridden by subclasses.
   *
   * @param magnitude the magnitude of the measurement.
   * @return the measurement object
   * @see #of(double)
   */
  public final Per<N, D> ofNative(double magnitude) {
    return new ImmutablePer<>(magnitude, toBaseUnits(magnitude), this);
  }

  /**
   * Creates a new immutable measurement of the given magnitude in terms of the ratio unit's base
   * unit. This will always return a {@code Per} object and cannot be overridden by subclasses.
   *
   * @param baseUnitMagnitude the magnitude of the measure in terms of its base units.
   * @return the measurement object
   * @see #ofBaseUnits(double)
   */
  public final Per<N, D> ofNativeBaseUnits(double baseUnitMagnitude) {
    return new ImmutablePer<>(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  @SuppressWarnings("unchecked")
  public Measure<? extends PerUnit<N, D>> zero() {
    return (Measure<? extends PerUnit<N, D>>) super.zero();
  }

  @Override
  @SuppressWarnings("unchecked")
  public Measure<? extends PerUnit<N, D>> one() {
    return (Measure<? extends PerUnit<N, D>>) super.one();
  }

  /**
   * {@inheritDoc}
   *
   * <p>Note: When called on an object of type {@code PerUnit} (and <i>not</i> a subclass!), this
   * method will always return a {@link edu.wpi.first.units.measure.MutPer} instance.
   *
   * @param initialMagnitude the starting magnitude of the measure
   * @return the ratio measure
   */
  @Override
  public MutableMeasure<? extends PerUnit<N, D>, ?, ?> mutable(double initialMagnitude) {
    return mutableNative(initialMagnitude);
  }

  /**
   * Creates a new mutable measurement of the given magnitude in terms of the ratio unit. This will
   * always return a {@code Per} object and cannot be overridden by subclasses.
   *
   * @param initialMagnitude the starting magnitude of the measure
   * @return the ratio measure
   * @see #mutable(double)
   */
  public final MutPer<N, D> mutableNative(double initialMagnitude) {
    return new MutPer<>(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  @Override
  public Unit per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, PerUnit<? extends N, ? extends D> otherUnit) {
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
    PerUnit<?, ?> perUnit = (PerUnit<?, ?>) o;
    return Objects.equals(m_numerator, perUnit.m_numerator)
        && Objects.equals(m_denominator, perUnit.m_denominator);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_numerator, m_denominator);
  }
}
