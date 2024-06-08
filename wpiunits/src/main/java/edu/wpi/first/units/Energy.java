// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of energy dimension.
 *
 * <p>This is the base type for units of energy dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;Energy&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Joules} and {@link Units#Kilojoules}) can be found in the
 * {@link Units} class.
 */
public class Energy extends Unit<Energy> {
  private final Force m_force;
  private final Distance m_distance;

  /**
   * Keep a cache of created instances so expressions like Newtons.times(Feet) don't do any
   * allocations after the first.
   */
  private static final LongToObjectHashMap<Energy> cache = new LongToObjectHashMap<>();

  Energy(
      Energy baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_force = baseUnit.getForce();
    m_distance = baseUnit.getDistance();
  }

  Energy(Energy baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
    m_force = baseUnit.getForce();
    m_distance = baseUnit.getDistance();
  }

  /**
   * Creates a new energy unit derived from the product of a force unit and a distance acceleration
   * unit.
   *
   * @param mass the mass unit
   * @param acceleration the linear acceleration unit.
   */
  Energy(Force force, Distance distance) {
    super(
        force.isBaseUnit() && distance.isBaseUnit()
            ? null
            : combine(force.getBaseUnit(), distance.getBaseUnit()),
        force.toBaseUnits(1) * distance.toBaseUnits(1),
        force.name() + "-" + distance.name(),
        force.symbol() + "*" + distance.symbol());
    m_force = force;
    m_distance = distance;
  }

  /**
   * Creates a new force unit derived from the product of a mass unit and a linear acceleration
   * unit.
   *
   * <pre>
   * Force.combine(Kilograms, FeetPerSecondPerSecond)
   * </pre>
   *
   * @param force the force unit
   * @param distance the distance unit
   * @return the energy unit
   */
  static Energy combine(Force force, Distance distance) {
    final long key =
        ((long) force.hashCode()) << 32L | (((long) distance.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new Energy(force, distance);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Creates a power unit derived from this one.
   *
   * <pre>
   *   Joules.per(Second) // Power
   *   Kilojoules.per(Minute) // Power
   * </pre>
   *
   * @param period the time period of the velocity, such as seconds or milliseconds
   * @return a linear velocity unit corresponding to the rate of change of distance over time
   */
  public Power per(Time period) {
    return Power.combine(this, period);
  }

  /**
   * Gets the force unit.
   *
   * @return the force unit
   */
  public Force getForce() {
    return m_force;
  }

  /**
   * Gets the distance unit.
   *
   * @return the distance unit
   */
  public Distance getDistance() {
    return m_distance;
  }

  @Override
  public String toString() {
    return "(" + m_force.toString() + " * " + m_distance.toString() + ")";
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
    Energy energy = (Energy) o;
    return Objects.equals(m_force, energy.getForce())
        && Objects.equals(m_distance, energy.getDistance());
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_force, m_distance);
  }
}
