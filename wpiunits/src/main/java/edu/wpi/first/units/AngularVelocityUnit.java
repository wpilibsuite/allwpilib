// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Watts;

import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.units.measure.ImmutableAngularVelocity;
import edu.wpi.first.units.measure.MutAngularVelocity;

/** A unit of angular velocity like {@link Units#RadiansPerSecond}. */
public final class AngularVelocityUnit extends PerUnit<AngleUnit, TimeUnit> {
  private static final CombinatoryUnitCache<AngleUnit, TimeUnit, AngularVelocityUnit> cache =
      new CombinatoryUnitCache<>(AngularVelocityUnit::new);

  AngularVelocityUnit(AngleUnit numerator, TimeUnit denominator) {
    super(
        numerator.isBaseUnit() && denominator.isBaseUnit()
            ? null
            : combine(numerator.getBaseUnit(), denominator.getBaseUnit()),
        numerator,
        denominator);
  }

  AngularVelocityUnit(
      AngularVelocityUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  /**
   * Combines an angle unit and a period of time into a unit of angular velocity.
   *
   * @param angle the unit of the changing angle
   * @param time the period of the changing angle
   * @return the combined angular velocity unit
   */
  public static AngularVelocityUnit combine(AngleUnit angle, TimeUnit time) {
    return cache.combine(angle, time);
  }

  @Override
  public AngularVelocityUnit getBaseUnit() {
    return (AngularVelocityUnit) super.getBaseUnit();
  }

  @Override
  public AngularVelocity of(double magnitude) {
    return new ImmutableAngularVelocity(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public AngularVelocity ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableAngularVelocity(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public AngularVelocity zero() {
    return (AngularVelocity) super.zero();
  }

  @Override
  public AngularVelocity one() {
    return (AngularVelocity) super.one();
  }

  @Override
  public MutAngularVelocity mutable(double initialMagnitude) {
    return new MutAngularVelocity(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  /**
   * Constructs a unit of power equivalent to this unit of angular velocity multiplied by another
   * unit of torque. For example, {@code NewtonMeters.times(RadiansPerSecond)} will return a unit of
   * power equivalent to one Watt.
   *
   * @param torque the unit of torque
   * @param name the name of the resulting unit of power
   * @param symbol the symbol used to represent the unit of power
   * @return the power unit
   */
  public PowerUnit mult(TorqueUnit torque, String name, String symbol) {
    double baseUnitEquivalent = torque.toBaseUnits(1) / this.toBaseUnits(1);
    UnaryFunction toBaseConverter = x -> x * baseUnitEquivalent;
    UnaryFunction fromBaseConverter = x -> x / baseUnitEquivalent;
    PowerUnit powerUnit = new PowerUnit(Watts, toBaseConverter, fromBaseConverter, name, symbol);
    return Units.derive(powerUnit).named(name).symbol(symbol).make();
  }

  @Override
  public AngularAccelerationUnit per(TimeUnit period) {
    return AngularAccelerationUnit.combine(this, period);
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<AngularVelocityUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, AngularVelocityUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }
}
