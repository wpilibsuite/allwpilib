// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of angular velocity dimension.
 *
 * <p>This is the base type for units of angular velocity dimension. It is also used to specify the
 * dimension for {@link Measure}: <code>Measure&lt;AngularVelocity&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#RadiansPerSecond} and {@link Units#DegreesPerSecond}) can
 * be found in the {@link Units} class.
 */
public class AngularVelocity extends Unit<AngularVelocity> {
  private final Angle m_angle;
  private final Time m_period;

  /**
   * Keep a cache of created instances so expressions like Degrees.per(Second) don't do any
   * allocations after the first.
   */
  private static final LongToObjectHashMap<AngularVelocity> cache = new LongToObjectHashMap<>();

  AngularVelocity(AngularVelocity baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
    m_angle = baseUnit.getAngle();
    m_period = baseUnit.getPeriod();
  }

  AngularVelocity(
      AngularVelocity baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_angle = baseUnit.getAngle();
    m_period = baseUnit.getPeriod();
  }

  /**
   * Creates a new angular velocity unit derived from the ratio of an angle unit to a time unit.
   * Consider using {@link #combine} instead of manually calling this constructor.
   *
   * @param angle the angle unit
   * @param period the period for unit time
   */
  AngularVelocity(Angle angle, Time period) {
    super(
        angle.isBaseUnit() && period.isBaseUnit()
            ? null
            : combine(angle.getBaseUnit(), period.getBaseUnit()),
        angle.toBaseUnits(1) / period.toBaseUnits(1),
        angle.name() + " per " + period.name(),
        angle.symbol() + "/" + period.symbol());
    m_angle = angle;
    m_period = period;
  }

  /**
   * Creates an angular acceleration unit derived from this one. Can be chained to denote angular
   * velocity, angular acceleration, angular jerk, etc.
   *
   * <pre>
   *   Radians.per(Second) // angular velocity
   *   Degrees.per(Second).per(Second).of(90) // 90 degrees per second
   * </pre>
   *
   * @param period the time period of the velocity, such as seconds or milliseconds
   * @return an angular velocity unit corresponding to the rate of change of angular velocity over
   *     time
   */
  public AngularAcceleration per(Time period) {
    return AngularAcceleration.combine(this, period);
  }

  /**
   * Creates a new angular velocity unit derived from an angle numerator and time denominator units.
   *
   * <pre>
   * AngularVelocity.combine(Degrees, Minute)
   * </pre>
   *
   * @param angle the angle unit
   * @param time the time unit
   * @return the angular velocity unit
   */
  static AngularVelocity combine(Angle angle, Time time) {
    final long key = ((long) angle.hashCode()) << 32L | (((long) time.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new AngularVelocity(angle, time);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Gets the angle unit.
   *
   * @return the angle unit
   */
  public Angle getAngle() {
    return m_angle;
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
   * Returns the reciprocal of this angular velocity.
   *
   * @return the reciprocal
   */
  public Per<Time, Angle> reciprocal() {
    return m_period.per(m_angle);
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
    AngularVelocity velocity = (AngularVelocity) o;
    return m_angle.equals(velocity.m_angle) && m_period.equals(velocity.m_period);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_angle, m_period);
  }
}
