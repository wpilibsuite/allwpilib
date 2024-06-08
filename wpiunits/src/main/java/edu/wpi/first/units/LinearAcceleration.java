// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of linear acceleration dimension.
 *
 * <p>This is the base type for units of linear acceleration dimension. It is also used to specify
 * the dimension for {@link Measure}: <code>Measure&lt;LinearAcceleration&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#MetersPerSecondPerSecond} and {@link
 * Units#FeetPerSecondPerSecond}) can be found in the {@link Units} class.
 */
public class LinearAcceleration extends Unit<LinearAcceleration> {
  private final LinearVelocity m_linearVelocity;
  private final Time m_period;

  /**
   * Keep a cache of created instances so expressions like MetersPerSecond.per(Second) don't do any
   * allocations after the first.
   */
  private static final LongToObjectHashMap<LinearAcceleration> cache = new LongToObjectHashMap<>();

  LinearAcceleration(
      LinearAcceleration baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
    m_linearVelocity = baseUnit.getLinearVelocity();
    m_period = baseUnit.getPeriod();
  }

  LinearAcceleration(
      LinearAcceleration baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_linearVelocity = baseUnit.getLinearVelocity();
    m_period = baseUnit.getPeriod();
  }

  /**
   * Creates a new linear acceleration unit derived from the ratio of an linear velocity unit to a
   * time unit. Consider using {@link #combine} instead of manually calling this constructor.
   *
   * @param linearVelocity the linear velocity unit
   * @param period the period for unit time
   */
  LinearAcceleration(LinearVelocity linearVelocity, Time period) {
    super(
        linearVelocity.isBaseUnit() && period.isBaseUnit()
            ? null
            : combine(linearVelocity.getBaseUnit(), period.getBaseUnit()),
        linearVelocity.toBaseUnits(1) / period.toBaseUnits(1),
        linearVelocity.name() + " per " + period.name(),
        linearVelocity.symbol() + "/" + period.symbol());
    m_linearVelocity = linearVelocity;
    m_period = period;
  }

  /**
   * Creates a new linear acceleration unit derived from an linear velocity numerator and time
   * denominator units.
   *
   * <pre>
   * LinearVelocity.combine(Degrees, Minute)
   * </pre>
   *
   * @param linearVelocity the linear velocity unit
   * @param time the time unit
   * @return the linear acceleration unit
   */
  static LinearAcceleration combine(LinearVelocity linearVelocity, Time time) {
    final long key =
        ((long) linearVelocity.hashCode()) << 32L | (((long) time.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new LinearAcceleration(linearVelocity, time);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Creates a force unit derived from this one.
   *
   * <pre>
   *   MetersPerSecondPerSecond.times(Kilograms) // force
   *   FeetPerSecondPerSecond.times(Grams) // force
   * </pre>
   *
   * @param mass the mass that is being accelerated.
   * @return a force unit corresponding to mass times linear acceleration
   */
  public Force times(Mass mass) {
    return Force.combine(mass, this);
  }

  /**
   * Gets the linear velocity unit.
   *
   * @return the linear velocity unit
   */
  public LinearVelocity getLinearVelocity() {
    return m_linearVelocity;
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
   * Returns the reciprocal of this linear acceleration.
   *
   * @return the reciprocal
   */
  public Per<Time, LinearVelocity> reciprocal() {
    return m_period.per(m_linearVelocity);
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
    LinearAcceleration velocity = (LinearAcceleration) o;
    return m_linearVelocity.equals(velocity.m_linearVelocity) && m_period.equals(velocity.m_period);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_linearVelocity, m_period);
  }
}
