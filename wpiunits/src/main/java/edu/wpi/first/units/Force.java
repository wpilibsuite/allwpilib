// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of the force dimension.
 *
 * <p>This is the base type for units of the force dimension. It is also used to specify the
 * dimension for {@link Measure}: <code>Measure&lt;Force&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Newtons} and {@link Units#PoundsForce}) can be found in the
 * {@link Units} class.
 */
public class Force extends Unit<Force> {
  private final Mass m_mass;
  private final LinearAcceleration m_acceleration;

  /**
   * Keep a cache of created instances so expressions like Kilograms.times(FeetPerSecondPerSecond)
   * don't do any allocations after the first.
   */
  private static final LongToObjectHashMap<Force> cache = new LongToObjectHashMap<>();

  Force(Force baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
    m_mass = baseUnit.getMass();
    m_acceleration = baseUnit.getAcceleration();
  }

  Force(
      Force baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_mass = baseUnit.getMass();
    m_acceleration = baseUnit.getAcceleration();
  }

  /**
   * Creates a new force unit derived from the product of a mass unit and a linear acceleration
   * unit. Consider using {@link #combine} instead of manually calling this constructor.
   *
   * @param mass the mass unit
   * @param acceleration the linear acceleration unit.
   */
  Force(Mass mass, LinearAcceleration acceleration) {
    super(
        mass.isBaseUnit() && acceleration.isBaseUnit()
            ? null
            : combine(mass.getBaseUnit(), acceleration.getBaseUnit()),
        mass.toBaseUnits(1) * acceleration.toBaseUnits(1),
        mass.name() + "-" + acceleration.name(),
        mass.symbol() + "*" + acceleration.symbol());
    m_mass = mass;
    m_acceleration = acceleration;
  }

  /**
   * Creates a new force unit derived from the product of a mass unit and a linear acceleration
   * unit.
   *
   * <pre>
   * Force.combine(Kilograms, FeetPerSecondPerSecond)
   * </pre>
   *
   * @param mass the mass unit
   * @param acceleration the linear acceleration unit
   * @return the force unit
   */
  static Force combine(Mass mass, LinearAcceleration acceleration) {
    final long key =
        ((long) mass.hashCode()) << 32L | (((long) acceleration.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new Force(mass, acceleration);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Creates an energy unit derived from this one.
   *
   * <pre>
   *   Newtons.times(Meters) // force
   *   Pound.times(Feet) // force
   * </pre>
   *
   * @param distance the distance the force was applied.
   * @return an energy unit corresponding to force times distance.
   */
  public Energy times(Distance distance) {
    return Energy.combine(this, distance);
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
   * Gets the linear acceleration unit.
   *
   * @return the linear acceleration unit
   */
  public LinearAcceleration getAcceleration() {
    return m_acceleration;
  }

  @Override
  public String toString() {
    return "(" + m_mass.toString() + " * " + m_acceleration.toString() + ")";
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
    Force force = (Force) o;
    return Objects.equals(m_mass, force.getMass())
        && Objects.equals(m_acceleration, force.getAcceleration());
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_mass, m_acceleration);
  }
}
