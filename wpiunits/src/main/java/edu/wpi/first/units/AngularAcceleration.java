// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of angular acceleration dimension.
 *
 * <p>This is the base type for units of angular acceleration dimension. It is also used to specify
 * the dimension for {@link Measure}: <code>Measure&lt;AngularAcceleration&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#RadiansPerSecondPerSecond} and {@link
 * Units#DegreesPerSecondPerSecond}) can be found in the {@link Units} class.
 */
public class AngularAcceleration extends Unit<AngularAcceleration> {
  private final AngularVelocity m_angularVelocity;
  private final Time m_period;

  /**
   * Keep a cache of created instances so expressions like DegreesPerSecond.per(Second) don't do any
   * allocations after the first.
   */
  private static final LongToObjectHashMap<AngularAcceleration> cache = new LongToObjectHashMap<>();

  AngularAcceleration(
      AngularAcceleration baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
    m_angularVelocity = baseUnit.getAngularVelocity();
    m_period = baseUnit.getPeriod();
  }

  AngularAcceleration(
      AngularAcceleration baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_angularVelocity = baseUnit.getAngularVelocity();
    m_period = baseUnit.getPeriod();
  }

  /**
   * Creates a new angular acceleration unit derived from the ratio of an angular velocity unit to a
   * time unit. Consider using {@link #combine} instead of manually calling this constructor.
   *
   * @param angularVelocity the angular velocity unit
   * @param period the period for unit time
   */
  protected AngularAcceleration(AngularVelocity angularVelocity, Time period) {
    super(
        angularVelocity.isBaseUnit() && period.isBaseUnit()
            ? null
            : combine(angularVelocity.getBaseUnit(), period.getBaseUnit()),
        angularVelocity.toBaseUnits(1) / period.toBaseUnits(1),
        angularVelocity.name() + " per " + period.name(),
        angularVelocity.symbol() + "/" + period.symbol());
    m_angularVelocity = angularVelocity;
    m_period = period;
  }

  /**
   * Creates a new angular acceleration unit derived from an angular velocity numerator and time
   * denominator units.
   *
   * <pre>
   * AngularVelocity.combine(Degrees, Minute)
   * </pre>
   *
   * <p>It's recommended to use the convenience function {@link AngularVelocity#per(Time)} instead
   * of calling this factory directly.
   *
   * @param angularVelocity the angular velocity unit
   * @param time the time unit
   * @return the angular acceleration unit
   */
  public static AngularAcceleration combine(AngularVelocity angularVelocity, Time time) {
    final long key =
        ((long) angularVelocity.hashCode()) << 32L | (((long) time.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new AngularAcceleration(angularVelocity, time);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Gets the angular velocity unit.
   *
   * @return the angular velocity unit
   */
  public AngularVelocity getAngularVelocity() {
    return m_angularVelocity;
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
   * Returns the reciprocal of this angular acceleration.
   *
   * @return the reciprocal
   */
  public Per<Time, AngularVelocity> reciprocal() {
    return m_period.per(m_angularVelocity);
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
    AngularAcceleration velocity = (AngularAcceleration) o;
    return m_angularVelocity.equals(velocity.m_angularVelocity)
        && m_period.equals(velocity.m_period);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_angularVelocity, m_period);
  }
}
