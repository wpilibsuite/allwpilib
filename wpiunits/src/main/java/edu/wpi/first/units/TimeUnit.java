// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * Unit of time dimension.
 *
 * <p>This is the base type for units of time dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;TimeUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Seconds} and {@link Units#Milliseconds}) can be found in
 * the {@link Units} class.
 */
public class TimeUnit extends Unit {
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

  public DimensionlessUnit per(TimeUnit other) {
    return Units.derive(Units.Value)
        .toBase(this.getConverterToBase().div(other.getConverterToBase()))
        .fromBase(other.getConverterFromBase().div(this.getConverterFromBase()))
        .named(this.name() + " per " + other.name())
        .symbol(this.symbol() + "/" + other.symbol())
        .make();
  }

  public <U extends Unit> Per<TimeUnit, U> per(U other) {
    return Per.combine(this, other);
  }

  public double convertFrom(double magnitude, TimeUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  public Time of(double magnitude) {
    return new Time(magnitude, toBaseUnits(magnitude), this);
  }
}
