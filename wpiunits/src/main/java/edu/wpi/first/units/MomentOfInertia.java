// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of moment of inertia dimension.
 *
 * <p>This is the base type for units of moment of inertia dimension. It is also used to specify the
 * dimension for {@link Measure}: <code>Measure&lt;MomentOfInertia&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#KilogramSquareMeters}.
 */
public class MomentOfInertia extends Unit<MomentOfInertia> {
  private final AngularMomentum m_angularMomentum;
  private final AngularVelocity m_angularVelocity;

  /**
   * Keep a cache of created instances so expressions like
   * KilogramMetersSquaredPerSecond.per(RadiansPerSecond) don't do any allocations after the first.
   */
  private static final LongToObjectHashMap<MomentOfInertia> cache = new LongToObjectHashMap<>();

  MomentOfInertia(MomentOfInertia baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
    m_angularMomentum = baseUnit.getAngularMomentum();
    m_angularVelocity = baseUnit.getAngularVelocity();
  }

  MomentOfInertia(
      MomentOfInertia baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_angularMomentum = baseUnit.getAngularMomentum();
    m_angularVelocity = baseUnit.getAngularVelocity();
  }

  /**
   * Creates a voltage unit derived from the ratio of a power unit to a current unit.
   *
   * @param power the power unit
   * @param current the unit of current.
   */
  MomentOfInertia(AngularMomentum angularMomentum, AngularVelocity angularVelocity) {
    super(
        angularMomentum.isBaseUnit() && angularVelocity.isBaseUnit()
            ? null
            : combine(angularMomentum.getBaseUnit(), angularVelocity.getBaseUnit()),
        angularMomentum.toBaseUnits(1) / angularVelocity.toBaseUnits(1),
        angularMomentum.name() + " per " + angularVelocity.name(),
        angularMomentum.symbol() + "/" + angularVelocity.symbol());
    m_angularMomentum = angularMomentum.getBaseUnit();
    m_angularVelocity = angularVelocity.getBaseUnit();
  }

  /**
   * Creates a new moment of inertia unit derived from an angular momentum numerator and angular
   * velocity denominator units.
   *
   * <pre>
   * MomentOfInertia.combine(KilogramMetersSquaredPerSecond, RadiansPerSecond)
   * </pre>
   *
   * @param angularMomentum the angular momentum unit
   * @param angularVelocity the angular velocity unit
   * @return the moment of inertia unit
   */
  static MomentOfInertia combine(AngularMomentum angularMomentum, AngularVelocity angularVelocity) {
    final long key =
        ((long) angularMomentum.hashCode()) << 32L
            | (((long) angularVelocity.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new MomentOfInertia(angularMomentum, angularVelocity);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Creates a torque unit derived from this one.
   *
   * <pre>
   *   KilogramMetersSquared.times(RadiansPerSecondPerSecond) // torque
   * </pre>
   *
   * @param angularAcceleration the angular acceleration acting on the moment of inertia
   * @return a torque unit corresponding to moment of inertia times angular acceleration
   */
  public Torque times(AngularAcceleration angularAcceleration) {
    return Torque.combine(this, angularAcceleration);
  }

  /**
   * Gets the angular momentum unit.
   *
   * @return the angular momentum unit
   */
  public AngularMomentum getAngularMomentum() {
    return m_angularMomentum;
  }

  /**
   * Gets the unit of angular velocity.
   *
   * @return the unit of angular velocity
   */
  public AngularVelocity getAngularVelocity() {
    return m_angularVelocity;
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
    MomentOfInertia momentOfInertia = (MomentOfInertia) o;
    return m_angularMomentum.equals(momentOfInertia.m_angularMomentum)
        && m_angularVelocity.equals(momentOfInertia.m_angularVelocity);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_angularMomentum, m_angularVelocity);
  }
}
