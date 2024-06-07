// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of the angular momentum dimension.
 *
 * <p>This is the base type for units of the angular momentum dimension. It is also used to specify
 * the dimension for {@link Measure}: <code>Measure&lt;AngularMomentum&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#KilogramMetersSquaredPerSecond}.
 */
public class AngularMomentum extends Unit<AngularMomentum> {
  private final Distance m_distance;
  private final LinearMomentum m_linearMomentum;

  /**
   * Keep a cache of created instances so expressions like Meters.times(KilogramMetersPerSecond)
   * don't do any allocations after the first.
   */
  private static final LongToObjectHashMap<AngularMomentum> cache = new LongToObjectHashMap<>();

  AngularMomentum(AngularMomentum baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
    m_distance = baseUnit.getDistance();
    m_linearMomentum = baseUnit.getLinearMomentum();
  }

  AngularMomentum(
      AngularMomentum baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_distance = baseUnit.getDistance();
    m_linearMomentum = baseUnit.getLinearMomentum();
  }

  /**
   * Creates a new angular momentum unit derived from the product of a distance unit and a linear
   * momentum unit. Consider using {@link #combine} instead of manually calling this constructor.
   *
   * @param distance the distance unit
   * @param linearMomentum the linear momentum unit.
   */
  AngularMomentum(Distance distance, LinearMomentum linearMomentum) {
    super(
        distance.isBaseUnit() && linearMomentum.isBaseUnit()
            ? null
            : combine(distance.getBaseUnit(), linearMomentum.getBaseUnit()),
        distance.toBaseUnits(1) * linearMomentum.toBaseUnits(1),
        distance.name() + "-" + linearMomentum.name(),
        distance.symbol() + "*" + linearMomentum.symbol());
    m_distance = distance;
    m_linearMomentum = linearMomentum;
  }

  /**
   * Creates a new angular momentum unit derived from the product of a distance unit and a linear
   * momentum unit.
   *
   * <pre>
   * AngularMomentum.combine(Meters, KilogramMetersPerSecond)
   * </pre>
   *
   * @param distance the distance unit
   * @param linearMomentum the linear momentum unit
   * @return the angular momentum unit
   */
  public static AngularMomentum combine(Distance distance, LinearMomentum linearMomentum) {
    final long key =
        ((long) distance.hashCode()) << 32L | (((long) linearMomentum.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new AngularMomentum(distance, linearMomentum);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Creates a moment of inertia unit derived from this one.
   *
   * <pre>
   *   KilogramMetersSquaredPerSecond.per(RadiansPerSecond) // moment of inertia
   * </pre>
   *
   * @param angularVelocity the angular velocity.
   * @return a moment of inertia unit corresponding to angular momentum per angular velocity.
   */
  public MomentOfInertia per(AngularVelocity angularVelocity) {
    return MomentOfInertia.combine(this, angularVelocity);
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
   * Gets the linear momentum unit.
   *
   * @return the linear momentum unit
   */
  public LinearMomentum getLinearMomentum() {
    return m_linearMomentum;
  }

  @Override
  public String toString() {
    return "(" + m_distance.toString() + " * " + m_linearMomentum.toString() + ")";
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
    AngularMomentum linearMomentum = (AngularMomentum) o;
    return Objects.equals(m_distance, linearMomentum.m_distance)
        && Objects.equals(m_linearMomentum, linearMomentum.m_linearMomentum);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_distance, m_linearMomentum);
  }
}
