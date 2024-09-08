// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableMass;
import edu.wpi.first.units.measure.Mass;
import edu.wpi.first.units.measure.MutMass;

/**
 * Unit of mass dimension.
 *
 * <p>This is the base type for units of mass dimension. It is also used to specify the dimension
 * for the mass-specific {@link Mass} measurement type.
 *
 * <p>Actual units (such as {@link Units#Grams} and {@link Units#Pounds}) can be found in the {@link
 * Units} class.
 */
public final class MassUnit extends Unit {
  /** Creates a new unit with the given name and multiplier to the base unit. */
  MassUnit(MassUnit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  MassUnit(
      MassUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  @Override
  public MassUnit getBaseUnit() {
    return (MassUnit) super.getBaseUnit();
  }

  @Override
  public Mass of(double magnitude) {
    return new ImmutableMass(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Mass ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableMass(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public Mass zero() {
    return (Mass) super.zero();
  }

  @Override
  public Mass one() {
    return (Mass) super.one();
  }

  @Override
  public MutMass mutable(double initialMagnitude) {
    return new MutMass(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  @Override
  public VelocityUnit<MassUnit> per(TimeUnit period) {
    return VelocityUnit.combine(this, period);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<MassUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another mass unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other mass unit
   * @param otherUnit the other mass unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, MassUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  /**
   * Multiplies this mass unit by a unit of linear velocity to form a combined unit of linear
   * momentum.
   *
   * @param velocity the unit of velocity
   * @return the combined unit of momentum
   */
  public LinearMomentumUnit mult(LinearVelocityUnit velocity) {
    return LinearMomentumUnit.combine(this, velocity);
  }

  /**
   * Multiplies this mass unit by a unit of linear acceleration to form a combined unit of force.
   *
   * @param acceleration the unit of acceleration
   * @return the combined unit of force
   */
  public ForceUnit mult(LinearAccelerationUnit acceleration) {
    return ForceUnit.combine(this, acceleration);
  }
}
