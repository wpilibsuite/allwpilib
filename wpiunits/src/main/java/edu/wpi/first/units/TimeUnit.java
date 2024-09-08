// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.ImmutableTime;
import edu.wpi.first.units.measure.MutTime;
import edu.wpi.first.units.measure.Time;

/**
 * Unit of time dimension.
 *
 * <p>This is the base type for units of time dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;TimeUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Seconds} and {@link Units#Milliseconds}) can be found in
 * the {@link Units} class.
 */
public final class TimeUnit extends Unit {
  TimeUnit(TimeUnit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  TimeUnit(
      TimeUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  @Override
  public TimeUnit getBaseUnit() {
    return (TimeUnit) super.getBaseUnit();
  }

  /**
   * Creates a dimensionless unit corresponding to the scale factor between this and another unit of
   * time.
   *
   * @param other the other unit of time
   * @return the result
   */
  @Override
  public DimensionlessUnit per(TimeUnit other) {
    return Units.derive(Units.Value)
        .toBase(this.getConverterToBase().div(other.getConverterToBase()))
        .fromBase(other.getConverterFromBase().div(this.getConverterFromBase()))
        .named(this.name() + " per " + other.name())
        .symbol(this.symbol() + "/" + other.symbol())
        .make();
  }

  /**
   * Creates a ratio unit between this unit and an arbitrary other unit.
   *
   * @param other the other unit
   * @param <U> the type of the other unit
   * @return the ratio unit
   */
  public <U extends Unit> PerUnit<TimeUnit, U> per(U other) {
    return PerUnit.combine(this, other);
  }

  /**
   * Converts a measurement value in terms of another unit to this unit.
   *
   * @param magnitude the magnitude of the measurement in terms of the other unit
   * @param otherUnit the other unit
   * @return the value of the measurement in terms of this unit
   */
  public double convertFrom(double magnitude, TimeUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  @Override
  public Time of(double magnitude) {
    return new ImmutableTime(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Time ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableTime(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public Time zero() {
    return (Time) super.zero();
  }

  @Override
  public Time one() {
    return (Time) super.one();
  }

  @Override
  public MutTime mutable(double initialMagnitude) {
    return new MutTime(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }
}
