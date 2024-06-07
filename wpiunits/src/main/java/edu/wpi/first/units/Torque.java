// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of the torque dimension.
 *
 * <p>This is the base type for units of the torque dimension. It is also used to specify the
 * dimension for {@link Measure}: <code>Measure&lt;Torque&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#NewtonMeters}.
 */
public class Torque extends Unit<Torque> {
  private final MomentOfInertia m_momentOfInertia;
  private final AngularAcceleration m_angularAcceleration;

  /**
   * Keep a cache of created instances so expressions like
   * KilogramMetersSquared.times(RadianPerSecondPerSecond) don't do any allocations after the first.
   */
  private static final LongToObjectHashMap<Torque> cache = new LongToObjectHashMap<>();

  Torque(Torque baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
    m_momentOfInertia = baseUnit.getMomentOfInertia();
    m_angularAcceleration = baseUnit.getAngularAcceleration();
  }

  Torque(
      Torque baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_momentOfInertia = baseUnit.getMomentOfInertia();
    m_angularAcceleration = baseUnit.getAngularAcceleration();
  }

  /**
   * Creates a new torque unit derived from the product of a moment of inertia unit and an angular
   * acceleration unit.
   *
   * @param momentOfInertia the moment of inertia unit
   * @param angularAcceleration the angular acceleration unit.
   */
  Torque(MomentOfInertia momentOfInertia, AngularAcceleration angularAcceleration) {
    super(
        momentOfInertia.isBaseUnit() && angularAcceleration.isBaseUnit()
            ? null
            : combine(momentOfInertia.getBaseUnit(), angularAcceleration.getBaseUnit()),
        momentOfInertia.toBaseUnits(1) * angularAcceleration.toBaseUnits(1),
        momentOfInertia.name() + "-" + angularAcceleration.name(),
        momentOfInertia.symbol() + "*" + angularAcceleration.symbol());
    m_momentOfInertia = momentOfInertia;
    m_angularAcceleration = angularAcceleration;
  }

  /**
   * Creates a new torque unit derived from the product of a moment of inertia unit and an angular
   * acceleration unit.
   *
   * <pre>
   * Torque.combine(KilogramMetersSquared, RadiansPerSecondPerSecond)
   * </pre>
   *
   * @param momentOfInertia the moment of inertia unit
   * @param angularAcceleration the angular acceleration unit.
   * @return the torque unit
   */
  static Torque combine(MomentOfInertia momentOfInertia, AngularAcceleration angularAcceleration) {
    final long key =
        ((long) momentOfInertia.hashCode()) << 32L
            | (((long) angularAcceleration.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new Torque(momentOfInertia, angularAcceleration);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Gets the moment of inertia unit.
   *
   * @return the moment of inertia unit
   */
  public MomentOfInertia getMomentOfInertia() {
    return m_momentOfInertia;
  }

  /**
   * Gets the angular acceleration unit.
   *
   * @return the angular acceleration unit
   */
  public AngularAcceleration getAngularAcceleration() {
    return m_angularAcceleration;
  }

  @Override
  public String toString() {
    return "(" + m_momentOfInertia.toString() + " * " + m_angularAcceleration.toString() + ")";
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
    Torque torque = (Torque) o;
    return Objects.equals(m_momentOfInertia, torque.m_momentOfInertia)
        && Objects.equals(m_angularAcceleration, torque.m_angularAcceleration);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_momentOfInertia, m_angularAcceleration);
  }
}
