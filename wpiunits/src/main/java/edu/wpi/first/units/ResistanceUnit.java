// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableResistance;
import edu.wpi.first.units.measure.MutResistance;
import edu.wpi.first.units.measure.Resistance;

/**
 * Unit of resistance dimension.
 *
 * <p>This is the base type for units of resistance dimension. It is also used to specify the
 * dimension for {@link Measure}: <code>Measure&lt;ResistanceUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Ohms} and {@link Units#KiloOhms}) can be found in the
 * {@link Units} class.
 */
public final class ResistanceUnit extends PerUnit<VoltageUnit, CurrentUnit> {
  private static final CombinatoryUnitCache<VoltageUnit, CurrentUnit, ResistanceUnit> cache =
      new CombinatoryUnitCache<>(ResistanceUnit::new);

  ResistanceUnit(VoltageUnit numerator, CurrentUnit denominator) {
    super(
        numerator.isBaseUnit() && denominator.isBaseUnit()
            ? null
            : combine(numerator.getBaseUnit(), denominator.getBaseUnit()),
        numerator,
        denominator);
  }

  ResistanceUnit(
      ResistanceUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines an voltage and a current unit to form a unit of resistance.
   *
   * @param voltage the unit of voltage
   * @param current the unit of current
   * @return the combined unit of resistance
   */
  public static ResistanceUnit combine(VoltageUnit voltage, CurrentUnit current) {
    return cache.combine(voltage, current);
  }

  @Override
  public ResistanceUnit getBaseUnit() {
    return (ResistanceUnit) super.getBaseUnit();
  }

  @Override
  public Resistance of(double magnitude) {
    return new ImmutableResistance(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Resistance ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableResistance(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public Resistance zero() {
    return (Resistance) super.zero();
  }

  @Override
  public Resistance one() {
    return (Resistance) super.one();
  }

  @Override
  public MutResistance mutable(double initialMagnitude) {
    return new MutResistance(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<ResistanceUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another power unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other power unit
   * @param otherUnit the other power unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, ResistanceUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
