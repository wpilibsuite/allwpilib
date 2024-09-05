// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableMult;
import edu.wpi.first.units.measure.Mult;
import edu.wpi.first.units.measure.MutMult;
import java.util.Objects;

/**
 * A combinatory unit type that is equivalent to the product of two other others. Note that
 * algebraic reduction is not possible in Java's generic type system, so {@code MultUnit<A, B>} is
 * not type-compatible with {@code MultUnit<B, A>}!
 *
 * @param <A> the type of the first unit in the result
 * @param <B> the type of the second unit in the result
 */
public class MultUnit<A extends Unit, B extends Unit> extends Unit {
  private final A m_unitA;
  private final B m_unitB;

  @SuppressWarnings("rawtypes")
  private static final CombinatoryUnitCache<Unit, Unit, MultUnit> cache =
      new CombinatoryUnitCache<>(MultUnit::new);

  /**
   * Creates a new product unit. Consider using {@link #combine} instead of manually calling this
   * constructor.
   *
   * @param a the first unit of the product
   * @param b the second unit of the product
   */
  private MultUnit(A a, B b) {
    super(
        a.isBaseUnit() && b.isBaseUnit() ? null : combine(a.getBaseUnit(), b.getBaseUnit()),
        a.toBaseUnits(1) * b.toBaseUnits(1),
        a.name() + "-" + b.name(),
        a.symbol() + "*" + b.symbol());
    m_unitA = a;
    m_unitB = b;
  }

  /**
   * Creates a new product unit. Subclasses of {@code MultUnit} should use this constructor.
   *
   * @param baseUnit the base unit. Set this to null if the unit being constructed is its own base
   *     unit
   * @param a the first unit of the product
   * @param b the second unit of the product
   */
  protected MultUnit(MultUnit<A, B> baseUnit, A a, B b) {
    super(
        baseUnit,
        a.toBaseUnits(1) * b.toBaseUnits(1),
        a.name() + "-" + b.name(),
        a.symbol() + "*" + b.symbol());
    m_unitA = a;
    m_unitB = b;
  }

  MultUnit(
      MultUnit<A, B> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_unitA = getBaseUnit().unitA();
    m_unitB = getBaseUnit().unitB();
  }

  /**
   * Creates a new MultUnit unit derived from two arbitrary units multiplied together.
   *
   * <pre>
   *   MultUnit.combine(Volts, Meters) // Volt-Meters
   * </pre>
   *
   * @param <A> the type of the first unit
   * @param <B> the type of the second unit
   * @param a the first unit
   * @param b the second unit
   * @return the combined unit
   */
  @SuppressWarnings("unchecked")
  public static <A extends Unit, B extends Unit> MultUnit<A, B> combine(A a, B b) {
    return cache.combine(a, b);
  }

  /**
   * {@inheritDoc}
   *
   * <p>Note: When called on an object of type {@code MultUnit} (and <i>not</i> a subclass!), this
   * method will always return a {@link edu.wpi.first.units.measure.Mult} instance. If you want to
   * avoid casting, use {@link #ofNativeBaseUnits(double)} that returns a {@code Per} instance
   * directly.
   *
   * @param magnitude the magnitude of the measure in terms of its base units.
   * @return the measurement object
   */
  @Override
  public Measure<? extends MultUnit<A, B>> of(double magnitude) {
    return ofNative(magnitude);
  }

  /**
   * {@inheritDoc}
   *
   * <p>Note: When called on an object of type {@code MultUnit} (and <i>not</i> a subclass!), this
   * method will always return a {@link edu.wpi.first.units.measure.Mult} instance. If you want to
   * avoid casting, use {@link #ofNativeBaseUnits(double)} that returns a {@code Per} instance
   * directly.
   *
   * @param baseUnitMagnitude the magnitude of the measure in terms of its base units.
   * @return the measurement object
   */
  @Override
  public Measure<? extends MultUnit<A, B>> ofBaseUnits(double baseUnitMagnitude) {
    return ofNativeBaseUnits(baseUnitMagnitude);
  }

  /**
   * Creates a new immutable measurement of the given magnitude in terms of this unit. This will
   * always return a {@code Mult} object and cannot be overridden by subclasses.
   *
   * @param magnitude the magnitude of the measurement.
   * @return the measurement object
   * @see #of(double)
   */
  public final Mult<A, B> ofNative(double magnitude) {
    return new ImmutableMult<>(magnitude, toBaseUnits(magnitude), this);
  }

  /**
   * Creates a new immutable measurement of the given magnitude in terms of the unit's base unit.
   * This will always return a {@code Mult} object and cannot be overridden by subclasses.
   *
   * @param baseUnitMagnitude the magnitude of the measure in terms of its base units.
   * @return the measurement object
   * @see #ofBaseUnits(double)
   */
  public final Mult<A, B> ofNativeBaseUnits(double baseUnitMagnitude) {
    return new ImmutableMult<>(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  @SuppressWarnings("unchecked")
  public Measure<? extends MultUnit<A, B>> zero() {
    return (Measure<? extends MultUnit<A, B>>) super.zero();
  }

  @Override
  @SuppressWarnings("unchecked")
  public Measure<? extends MultUnit<A, B>> one() {
    return (Measure<? extends MultUnit<A, B>>) super.one();
  }

  @Override
  public MutableMeasure<? extends MultUnit<A, B>, ?, ?> mutable(double initialMagnitude) {
    return new MutMult<>(initialMagnitude, toBaseUnits(initialMagnitude), this);
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
  public double convertFrom(double magnitude, MultUnit<A, B> otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  @Override
  @SuppressWarnings("unchecked")
  public MultUnit<A, B> getBaseUnit() {
    return (MultUnit<A, B>) super.getBaseUnit();
  }

  /**
   * Gets the first unit of the product.
   *
   * @return the first unit
   */
  public A unitA() {
    return m_unitA;
  }

  /**
   * Gets the second unit of the product.
   *
   * @return the second unit
   */
  public B unitB() {
    return m_unitB;
  }

  @Override
  public String toString() {
    return "(" + m_unitA.toString() + " * " + m_unitB.toString() + ")";
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
    MultUnit<?, ?> multUnit = (MultUnit<?, ?>) o;
    return Objects.equals(m_unitA, multUnit.m_unitA) && Objects.equals(m_unitB, multUnit.m_unitB);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_unitA, m_unitB);
  }
}
