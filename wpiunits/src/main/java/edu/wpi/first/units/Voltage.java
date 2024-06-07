// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Watts;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * Unit of electric voltage dimension.
 *
 * <p>This is the base type for units of voltage dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;Voltage&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Volts} and {@link Units#Millivolts}) can be found in the
 * {@link Units} class.
 */
public class Voltage extends Unit<Voltage> {
  private final Power m_power;
  private final Current m_current;

  /**
   * Keep a cache of created instances so expressions like DegreesPerSecond.per(Second) don't do any
   * allocations after the first.
   */
  private static final LongToObjectHashMap<Voltage> cache = new LongToObjectHashMap<>();

  Voltage(Voltage baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
    m_power = baseUnit.getPower();
    m_current = baseUnit.getCurrent();
  }

  Voltage(
      Voltage baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
    m_power = baseUnit.getPower();
    m_current = baseUnit.getCurrent();
  }

  /**
   * Creates a voltage unit derived from the ratio of a power unit to a current unit.
   *
   * @param power the power unit
   * @param current the unit of current.
   */
  Voltage(Power power, Current current) {
    super(
        power.isBaseUnit() && current.isBaseUnit()
            ? null
            : combine(power.getBaseUnit(), current.getBaseUnit()),
        power.toBaseUnits(1) / current.toBaseUnits(1),
        power.name() + " per " + current.name(),
        power.symbol() + "/" + current.symbol());
    m_power = power;
    m_current = current;
  }

  /**
   * Creates a new power unit derived from an energy numerator and time denominator units.
   *
   * <pre>
   * Power.combine(Joules, Minute)
   * </pre>
   *
   * <p>It's recommended to use the convenience function {@link Energy#per(Time)} instead of calling
   * this factory directly.
   *
   * @param power the energy unit
   * @param current the time unit
   * @return the power unit
   */
  public static Voltage combine(Power power, Current current) {
    final long key = ((long) power.hashCode()) << 32L | (((long) current.hashCode()) & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = new Voltage(power, current);
    cache.put(key, newUnit);
    return newUnit;
  }

  /**
   * Gets the power unit.
   *
   * @return the energy unit
   */
  public Power getPower() {
    return m_power;
  }

  /**
   * Gets the unit of current.
   *
   * @return the unit of current
   */
  public Current getCurrent() {
    return m_current;
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
    Voltage voltage = (Voltage) o;
    return m_power.equals(voltage.m_power) && m_current.equals(voltage.m_current);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_power, m_current);
  }

  /**
   * Constructs a unit of power equivalent to this unit of voltage multiplied by another unit of
   * electrical current. For example, {@code Volts.times(Amps)} will return a unit of power
   * equivalent to one Watt; {@code Volts.times(Milliams)} will return a unit of power equivalent to
   * a milliwatt, and so on.
   *
   * @param current the current unit to multiply by
   * @param name the name of the resulting unit of power
   * @param symbol the symbol used to represent the unit of power
   * @return the power unit
   */
  public Power times(Unit<Current> current, String name, String symbol) {
    return new Power(Watts, toBaseUnits(1) * current.toBaseUnits(1), name, symbol);
  }
}
