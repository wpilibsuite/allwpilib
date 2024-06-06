// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of linear velocity dimension.
 *
 * <p>This is the base type for units of linear velocity dimension. It is also used to specify the
 * dimension for {@link Measure}: <code>Measure&lt;LinearVelocity&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#MetersPerSecond} and {@link Units#FeetPerSecond}) can be
 * found in the {@link Units} class.
 */
public class LinearVelocity extends Unit<LinearVelocity> {
  private final Distance m_distance;
  private final Time m_period;

  /**
   * Keep a cache of created instances so expressions like Meters.per(Second) don't do any
   * allocations after the first.
   */
  private static final LongToObjectHashMap<LinearVelocity> cache = new LongToObjectHashMap<>();

  /**
   * Creates a new linear velocity unit derived from the ratio of a distance unit to a time unit.
   * Consider using {@link #combine} instead of manually calling this constructor.
   *
   * @param distance the distance unit
   * @param period the period for unit time
   */
  protected LinearVelocity(Distance distance, Time period) {
    super(
        distance.isBaseUnit() && period.isBaseUnit()
            ? null
            : combine(distance.getBaseUnit(), period.getBaseUnit()),
        distance.toBaseUnits(1) / period.toBaseUnits(1),
        distance.name() + " per " + period.name(),
        distance.symbol() + "/" + period.symbol());
    m_distance = distance;
    m_period = period;
  }

  /**
   * Creates a linear acceleration unit derived from this one. Can be chained to denote velocity,
   * acceleration, jerk, etc.
   *
   * <pre>
   *   MetersPerSecond.per(Second) // linear acceleration
   *   Feet.per(Second).per(Second).of(32) // roughly 1G of acceleration
   * </pre>
   *
   * @param period the time period of the acceleration, such as seconds or milliseconds
   * @return a linear acceleration unit corresponding to the rate of change of linear velocity over
   *     time
   */
  public LinearAcceleration per2(Time period) {
    return LinearAcceleration.combine(this, period);
  }

  /**
   * Creates a new linear velocity unit derived from a distance numerator and time denominator
   * units.
   *
   * <pre>
   * LinearVelocity.combine(Feet, Minute)
   * </pre>
   *
   * <p>It's recommended to use the convenience function {@link Distance#per(Time)} instead of
   * calling this factory directly.
   *
   * @param distance the distance unit
   * @param time the time unit
   * @return the linear velocity unit
   */
  public static LinearVelocity combine(Distance distance, Time time) {
    final long key = ((long) distance.hashCode()) << 32L | (((long) time.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new LinearVelocity(distance, time);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Gets the distance unit.
   *
   * @return the distance unit
   */
  public Distance getDistance() {
    return m_distance;
  }

  /**
   * Gets the period unit.
   *
   * @return the period unit
   */
  public Time getPeriod() {
    return m_period;
  }

  /**
   * Returns the reciprocal of this linear velocity.
   *
   * @return the reciprocal
   */
  public Per<Time, Distance> reciprocal() {
    return m_period.per(m_distance);
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
    LinearVelocity velocity = (LinearVelocity) o;
    return m_distance.equals(velocity.m_distance) && m_period.equals(velocity.m_period);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_distance, m_period);
  }
}
