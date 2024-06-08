// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of power dimension.
 *
 * <p>This is the base type for units of power dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;Power&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Watts} and {@link Units#Horsepower}) can be found in the
 * {@link Units} class.
 */
public class Power extends Unit<Power> {
  private final Energy m_energy;
  private final Time m_period;

  /**
   * Keep a cache of created instances so expressions like DegreesPerSecond.per(Second) don't do any
   * allocations after the first.
   */
  private static final LongToObjectHashMap<Power> cache = new LongToObjectHashMap<>();

  Power(Power baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
    m_energy = baseUnit.getEnergy();
    m_period = baseUnit.getPeriod();
  }

  Power(
      Power baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_energy = baseUnit.getEnergy();
    m_period = baseUnit.getPeriod();
  }

  /**
   * Creates a power unit derived from the ratio of an energy unit to a time unit.
   *
   * @param energy the energy unit
   * @param period the period for unit time
   */
  Power(Energy energy, Time period) {
    super(
        energy.isBaseUnit() && period.isBaseUnit()
            ? null
            : combine(energy.getBaseUnit(), period.getBaseUnit()),
        energy.toBaseUnits(1) / period.toBaseUnits(1),
        energy.name() + " per " + period.name(),
        energy.symbol() + "/" + period.symbol());
    m_energy = energy;
    m_period = period;
  }

  /**
   * Creates a new power unit derived from an energy numerator and time denominator units.
   *
   * <pre>
   * Power.combine(Joules, Minute)
   * </pre>
   *
   * @param energy the energy unit
   * @param time the time unit
   * @return the power unit
   */
  static Power combine(Energy energy, Time time) {
    final long key = ((long) energy.hashCode()) << 32L | (((long) time.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new Power(energy, time);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Creates a voltage unit derived from this one.
   *
   * <pre>
   *   Watts.per(Amps) // voltage
   *   Horsepower.per(Milliamps) // voltage
   * </pre>
   *
   * @param current the current.
   * @return a voltage unit in terms of power per current.
   */
  public Voltage per(Current current) {
    return Voltage.combine(this, current);
  }

  /**
   * Gets the energy unit.
   *
   * @return the energy unit
   */
  public Energy getEnergy() {
    return m_energy;
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
   * Returns the reciprocal of this energy unit.
   *
   * @return the reciprocal
   */
  public Per<Time, Energy> reciprocal() {
    return m_period.per(m_energy);
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
    Power power = (Power) o;
    return m_energy.equals(power.m_energy) && m_period.equals(power.m_period);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_energy, m_period);
  }
}
