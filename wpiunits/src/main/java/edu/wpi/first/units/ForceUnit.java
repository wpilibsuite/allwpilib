// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.Force;
import edu.wpi.first.units.measure.ImmutableForce;
import edu.wpi.first.units.measure.MutForce;

/** A unit of force like {@link Units#Newtons}. */
public final class ForceUnit extends MultUnit<MassUnit, LinearAccelerationUnit> {
  private static final CombinatoryUnitCache<MassUnit, LinearAccelerationUnit, ForceUnit> cache =
      new CombinatoryUnitCache<>(ForceUnit::new);

  ForceUnit(MassUnit mass, LinearAccelerationUnit acceleration) {
    super(
        mass.isBaseUnit() && acceleration.isBaseUnit()
            ? null
            : combine(mass.getBaseUnit(), acceleration.getBaseUnit()),
        mass,
        acceleration);
  }

  ForceUnit(
      ForceUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a mass and (linear) acceleration to form a unit of force.
   *
   * @param mass the unit of mass
   * @param acceleration the unit of acceleration
   * @return the combined unit of force
   */
  public static ForceUnit combine(MassUnit mass, LinearAccelerationUnit acceleration) {
    return cache.combine(mass, acceleration);
  }

  @Override
  public ForceUnit getBaseUnit() {
    return (ForceUnit) super.getBaseUnit();
  }

  /**
   * Multiplies this force unit by a unit of distance to create a unit of torque.
   *
   * @param distance the unit of distance
   * @return the combined torque unit
   */
  public TorqueUnit multAsTorque(DistanceUnit distance) {
    return TorqueUnit.combine(distance, this);
  }

  // TODO: Add a multAsEnergy equivalent

  @Override
  public Force of(double magnitude) {
    return new ImmutableForce(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Force ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableForce(toBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public Force zero() {
    return (Force) super.zero();
  }

  @Override
  public Force one() {
    return (Force) super.one();
  }

  @Override
  public MutForce mutable(double initialMagnitude) {
    return new MutForce(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  @Override
  public VelocityUnit<ForceUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<ForceUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, ForceUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
