// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of velocity dimension that is a combination of a distance unit (numerator) and a time unit
 * (denominator).
 *
 * <p>This is the base type for units of velocity dimension. It is also used in combination with a
 * distance dimension to specify the dimension for {@link Measure}. For example: <code>
 * Measure&lt;Velocity&lt;Distance&gt;&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#MetersPerSecond} and {@link Units#RPM}) can be found in the
 * {@link Units} class.
 *
 * @param <D> the distance unit, such as {@link Angle} or {@link Distance}
 */
public class Velocity<D extends Unit<D>> extends Unit<Velocity<D>> {
  private final D m_unit;
  private final Time m_period;

  /**
   * Stores velocity units that were created ad-hoc using {@link #combine(Unit, Time, String,
   * String)}. Does not store objects created directly by constructors.
   */
  @SuppressWarnings("rawtypes")
  private static final LongToObjectHashMap<Velocity> cache = new LongToObjectHashMap<>();

  /** Generates a cache key used for cache lookups. */
  private static long cacheKey(Unit<?> numerator, Unit<?> denominator) {
    return ((long) numerator.hashCode()) << 32L | (((long) denominator.hashCode()) & 0xFFFFFFFFL);
  }

  /**
   * Creates a new velocity unit derived from an arbitrary numerator and time period units.
   *
   * <p>Results of this method are cached so future invocations with the same arguments will return
   * the pre-existing units instead of generating new identical ones.
   *
   * <pre>
   *   Velocity.combine(Kilograms, Second) // mass flow
   *   Velocity.combine(Feet, Millisecond) // linear speed
   *   Velocity.combine(Radians, Second) // angular speed
   *
   *   Velocity.combine(Feet.per(Second), Second) // linear acceleration in ft/s/s
   *   Velocity.combine(Radians.per(Second), Second) // angular acceleration
   * </pre>
   *
   * <p>It's recommended to use the convenience function {@link Unit#per(Time)} instead of calling
   * this factory directly.
   *
   * @param <D> the type of the numerator unit
   * @param numerator the numerator unit
   * @param period the period for unit time
   * @param name the name of the new velocity unit
   * @param symbol the symbol of the new velocity unit
   * @return the new unit
   */
  @SuppressWarnings("unchecked")
  public static <D extends Unit<D>> Velocity<D> combine(
      Unit<D> numerator, Time period, String name, String symbol) {
    long key = cacheKey(numerator, period);
    if (cache.containsKey(key)) {
      return cache.get(key);
    }

    Velocity<D> velocity = new Velocity<>((D) numerator, period, name, symbol);
    cache.put(key, velocity);
    return velocity;
  }

  /**
   * Creates a new velocity unit derived from an arbitrary numerator and time period units.
   *
   * <p>Results of this method are cached so future invocations with the same arguments will return
   * the pre-existing units instead of generating new identical ones.
   *
   * <p>This method automatically generates a new name and symbol for the new velocity unit.
   *
   * <pre>
   *   Velocity.combine(Kilograms, Second) // mass flow
   *   Velocity.combine(Feet, Millisecond) // linear speed
   *   Velocity.combine(Radians, Second) // angular speed
   *
   *   Velocity.combine(Feet.per(Second), Second) // linear acceleration in ft/s/s
   *   Velocity.combine(Radians.per(Second), Second) // angular acceleration
   * </pre>
   *
   * <p>It's recommended to use the convenience function {@link Unit#per(Time)} instead of calling
   * this factory directly.
   *
   * @param <D> the type of the numerator unit
   * @param numerator the numerator unit
   * @param period the period for unit time
   * @return the new unit
   */
  @SuppressWarnings("unchecked")
  public static <D extends Unit<D>> Velocity<D> combine(Unit<D> numerator, Time period) {
    long key = cacheKey(numerator, period);
    if (cache.containsKey(key)) {
      return cache.get(key);
    }

    var name = numerator.name() + " per " + period.name();
    var symbol = numerator.symbol() + "/" + period.symbol();

    Velocity<D> velocity = new Velocity<>((D) numerator, period, name, symbol);
    cache.put(key, velocity);
    return velocity;
  }

  @SuppressWarnings({"unchecked", "rawtypes"})
  Velocity(D unit, Time period, String name, String symbol) {
    super((Class) Velocity.class, unit.toBaseUnits(1) / period.toBaseUnits(1), name, symbol);
    this.m_unit = unit;
    this.m_period = period;
  }

  @SuppressWarnings({"unchecked", "rawtypes"})
  Velocity(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super((Class) Velocity.class, toBaseConverter, fromBaseConverter, name, symbol);
    this.m_unit = Units.anonymous();
    this.m_period = Units.Seconds;
  }

  /**
   * Gets the major unit being measured (eg Meters for Meters per Second).
   *
   * @return the major unit
   */
  public D getUnit() {
    return m_unit;
  }

  /**
   * Gets the period unit of the velocity, eg Seconds or Milliseconds.
   *
   * @return the period unit
   */
  public Time getPeriod() {
    return m_period;
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
    Velocity<?> velocity = (Velocity<?>) o;
    return m_unit.equals(velocity.m_unit) && m_period.equals(velocity.m_period);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_unit, m_period);
  }
}
