// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.units;

import org.wpilib.units.measure.Charge;

/**
 * Unit of electric charge dimension.
 *
 * <p>This is the base type for units of charge dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;ChargeUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Coulombs}) can be found in the {@link Units} class.
 */
public final class ChargeUnit extends MultUnit<CurrentUnit, TimeUnit> {
  private static final CombinatoryUnitCache<CurrentUnit, TimeUnit, ChargeUnit> cache =
      new CombinatoryUnitCache<>(ChargeUnit::new);

  ChargeUnit(CurrentUnit current, TimeUnit time) {
    super(
        current.isBaseUnit() && time.isBaseUnit()
            ? null
            : combine(current.getBaseUnit(), time.getBaseUnit()),
        current,
        time);
  }

  ChargeUnit(
      ChargeUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a current and time to form a unit of electrical charge.
   *
   * @param current the unit of current
   * @param time the unit of time
   * @return the combined unit of electrical charge
   */
  public static ChargeUnit combine(CurrentUnit current, TimeUnit time) {
    return cache.combine(current, time);
  }

  @Override
  public ChargeUnit getBaseUnit() {
    return (ChargeUnit) super.getBaseUnit();
  }

  @Override
  public Charge of(double magnitude) {
    return new Charge(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Charge ofBaseUnits(double baseUnitMagnitude) {
    return new Charge(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public Charge zero() {
    return (Charge) super.zero();
  }

  @Override
  public Charge one() {
    return (Charge) super.one();
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<ChargeUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other electrical charge unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, ChargeUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
