// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.Energy;
import edu.wpi.first.units.measure.ImmutableEnergy;
import edu.wpi.first.units.measure.MutEnergy;

/**
 * Unit of energy dimension.
 *
 * <p>This is the base type for units of energy dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;EnergyUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Joules} and {@link Units#Kilojoules}) can be found in the
 * {@link Units} class.
 */
public final class EnergyUnit extends Unit {
  EnergyUnit(
      EnergyUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  EnergyUnit(EnergyUnit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  @Override
  public EnergyUnit getBaseUnit() {
    return (EnergyUnit) super.getBaseUnit();
  }

  /**
   * Combines this unit of energy with a unit of time to create a unit of power.
   *
   * @param period the period of the change in energy
   * @return the combined unit of power
   */
  @Override
  public PowerUnit per(TimeUnit period) {
    return PowerUnit.combine(this, period);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<EnergyUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, EnergyUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  @Override
  public Energy of(double magnitude) {
    return new ImmutableEnergy(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Energy ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableEnergy(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public Energy zero() {
    return (Energy) super.zero();
  }

  @Override
  public Energy one() {
    return (Energy) super.one();
  }

  @Override
  public MutEnergy mutable(double initialMagnitude) {
    return new MutEnergy(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }
}
