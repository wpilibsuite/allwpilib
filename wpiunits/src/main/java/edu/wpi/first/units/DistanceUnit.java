// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.ImmutableDistance;
import edu.wpi.first.units.measure.MutDistance;

/**
 * Unit of linear dimension.
 *
 * <p>This is the base type for units of linear dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;DistanceUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Meters} and {@link Units#Inches}) can be found in the
 * {@link Units} class.
 */
public final class DistanceUnit extends Unit {
  DistanceUnit(DistanceUnit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  DistanceUnit(
      DistanceUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  @Override
  public DistanceUnit getBaseUnit() {
    return (DistanceUnit) super.getBaseUnit();
  }

  @Override
  public LinearVelocityUnit per(TimeUnit period) {
    return LinearVelocityUnit.combine(this, period);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<DistanceUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another distance unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other distance unit
   * @param otherUnit the other distance unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, DistanceUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  @Override
  public Distance of(double magnitude) {
    return new ImmutableDistance(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Distance ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableDistance(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  @SuppressWarnings("unchecked")
  public Measure<DistanceUnit> zero() {
    return (Measure<DistanceUnit>) super.zero();
  }

  @Override
  @SuppressWarnings("unchecked")
  public Measure<DistanceUnit> one() {
    return (Measure<DistanceUnit>) super.one();
  }

  @Override
  public MutDistance mutable(double initialMagnitude) {
    return new MutDistance(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  /**
   * Multiplies this distance unit by a unit of force to create a unit of torque.
   *
   * <p>Note: because torque and energy have the same dimensions (force multiplied by distance), we
   * cannot define methods with the same name for both. Instead, use {@link
   * ForceUnit#mult(DistanceUnit)} if you want a unit of energy.
   *
   * @param force the unit of force
   * @return the combined torque unit
   */
  public TorqueUnit mult(ForceUnit force) {
    return TorqueUnit.combine(this, force);
  }
}
