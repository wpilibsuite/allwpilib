// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Watts;

import edu.wpi.first.units.measure.ImmutableTorque;
import edu.wpi.first.units.measure.MutTorque;
import edu.wpi.first.units.measure.Torque;

/** A unit of torque like {@link edu.wpi.first.units.Units#NewtonMeters}. */
public final class TorqueUnit extends MultUnit<DistanceUnit, ForceUnit> {
  private static final CombinatoryUnitCache<DistanceUnit, ForceUnit, TorqueUnit> cache =
      new CombinatoryUnitCache<>(TorqueUnit::new);

  TorqueUnit(DistanceUnit distanceUnit, ForceUnit forceUnit) {
    super(
        distanceUnit.isBaseUnit() && forceUnit.isBaseUnit()
            ? null
            : combine(distanceUnit.getBaseUnit(), forceUnit.getBaseUnit()),
        distanceUnit,
        forceUnit);
  }

  TorqueUnit(
      MultUnit<DistanceUnit, ForceUnit> baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines a unit of distance and force to create a unit of torque.
   *
   * @param distance the distance unit
   * @param force the force unit
   * @return the combined torque unit
   */
  public static TorqueUnit combine(DistanceUnit distance, ForceUnit force) {
    return cache.combine(distance, force);
  }

  @Override
  public TorqueUnit getBaseUnit() {
    return (TorqueUnit) super.getBaseUnit();
  }

  @Override
  public Torque of(double magnitude) {
    return new ImmutableTorque(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Torque ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableTorque(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public Torque zero() {
    return (Torque) super.zero();
  }

  @Override
  public Torque one() {
    return (Torque) super.one();
  }

  @Override
  public MutTorque mutable(double initialMagnitude) {
    return new MutTorque(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  /**
   * Constructs a unit of power equivalent to this unit of torque multiplied by another unit of
   * angular velocity. For example, {@code NewtonMeters.times(RadiansPerSecond)} will return a unit
   * of power equivalent to one Watt.
   *
   * @param angularVelocity the unit of angular velocity
   * @param name the name of the resulting unit of power
   * @param symbol the symbol used to represent the unit of power
   * @return the power unit
   */
  public PowerUnit mult(AngularVelocityUnit angularVelocity, String name, String symbol) {
    double baseUnitEquivalent = this.toBaseUnits(1) / angularVelocity.toBaseUnits(1);
    UnaryFunction toBaseConverter = x -> x * baseUnitEquivalent;
    UnaryFunction fromBaseConverter = x -> x / baseUnitEquivalent;
    PowerUnit powerUnit = new PowerUnit(Watts, toBaseConverter, fromBaseConverter, name, symbol);
    return Units.derive(powerUnit).named(name).symbol(symbol).make();
  }

  @Override
  public VelocityUnit<TorqueUnit> per(TimeUnit time) {
    return VelocityUnit.combine(this, time);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<TorqueUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, TorqueUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
