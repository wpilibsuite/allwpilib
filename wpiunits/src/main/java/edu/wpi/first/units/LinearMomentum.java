// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of the linear momentum dimension.
 *
 * <p>This is the base type for units of the linear momentum dimension. It is also used to specify
 * the dimension for {@link Measure}: <code>Measure&lt;LinearMomentum&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#KilogramMetersPerSecond}.
 */
public class LinearMomentum extends Unit<LinearMomentum> {
  private final Mass m_mass;
  private final LinearVelocity m_linearVelocity;

  /**
   * Keep a cache of created instances so expressions like Kilograms.times(FeetPerSecond) don't do
   * any allocations after the first.
   */
  private static final LongToObjectHashMap<LinearMomentum> cache = new LongToObjectHashMap<>();

  LinearMomentum(LinearMomentum baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
    m_mass = baseUnit.getMass();
    m_linearVelocity = baseUnit.getVelocity();
  }

  LinearMomentum(
      LinearMomentum baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_mass = baseUnit.getMass();
    m_linearVelocity = baseUnit.getVelocity();
  }

  /**
   * Creates a new linear momentum unit derived from the product of a mass unit and a linear
   * velocity unit. Consider using {@link #combine} instead of manually calling this constructor.
   *
   * @param mass the mass unit
   * @param velocity the linear velocity unit.
   */
  LinearMomentum(Mass mass, LinearVelocity velocity) {
    super(
        mass.isBaseUnit() && velocity.isBaseUnit()
            ? null
            : combine(mass.getBaseUnit(), velocity.getBaseUnit()),
        mass.toBaseUnits(1) * velocity.toBaseUnits(1),
        mass.name() + "-" + velocity.name(),
        mass.symbol() + "*" + velocity.symbol());
    m_mass = mass;
    m_linearVelocity = velocity;
  }

  /**
   * Creates a new linear momentum unit derived from the product of a mass unit and a linear
   * velocity unit.
   *
   * <pre>
   * LinearMomentum.combine(Kilograms, FeetPerSecond)
   * </pre>
   *
   * @param mass the mass unit
   * @param velocity the linear velocity unit
   * @return the linear momentum unit
   */
  public static LinearMomentum combine(Mass mass, LinearVelocity velocity) {
    final long key = ((long) mass.hashCode()) << 32L | (((long) velocity.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new LinearMomentum(mass, velocity);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Creates an angular momentum unit derived from this one.
   *
   * <pre>
   *   KilogramMetersPerSecond.times(Meters) // angular momentum
   * </pre>
   *
   * @param position the position that the linear momentum is acting at.
   * @return an angular momentum unit corresponding to distance times linear momentum.
   */
  public AngularMomentum times(Distance position) {
    return AngularMomentum.combine(position, this);
  }

  /**
   * Gets the mass unit.
   *
   * @return the mass unit
   */
  public Mass getMass() {
    return m_mass;
  }

  /**
   * Gets the linear velocity unit.
   *
   * @return the linear velocity unit
   */
  public LinearVelocity getVelocity() {
    return m_linearVelocity;
  }

  @Override
  public String toString() {
    return "(" + m_mass.toString() + " * " + m_linearVelocity.toString() + ")";
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
    LinearMomentum linearMomentum = (LinearMomentum) o;
    return Objects.equals(m_mass, linearMomentum.getMass())
        && Objects.equals(m_linearVelocity, linearMomentum.m_linearVelocity);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_mass, m_linearVelocity);
  }
}
