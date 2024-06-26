// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Unit;
import java.util.Objects;

public abstract class MutableMeasureBase<
        U extends Unit, Base extends Measure<U>, MutSelf extends MutableMeasure<U, Base, MutSelf>>
    implements Measure<U>, MutableMeasure<U, Base, MutSelf> {
  protected double magnitude;
  protected double baseUnitMagnitude;
  protected U unit;

  public MutableMeasureBase(double magnitude, double baseUnitMagnitude, U unit) {
    this.magnitude = magnitude;
    this.baseUnitMagnitude = baseUnitMagnitude;
    this.unit = Objects.requireNonNull(unit, "Unit cannot be null");
  }

  @Override
  public double magnitude() {
    return magnitude;
  }

  @Override
  public double baseUnitMagnitude() {
    return baseUnitMagnitude;
  }

  @Override
  public U unit() {
    return unit;
  }

  @Override
  public String toString() {
    return toShortString();
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (!(o instanceof MutableMeasureBase<?, ?, ?> that)) return false;
    return Double.compare(magnitude, that.magnitude) == 0
        && Double.compare(baseUnitMagnitude, that.baseUnitMagnitude) == 0
        && Objects.equals(unit, that.unit);
  }

  @Override
  public int hashCode() {
    return Objects.hash(magnitude, baseUnitMagnitude, unit);
  }

  @SuppressWarnings("unchecked")
  @Override
  public MutSelf mut_replace(double magnitude, U newUnit) {
    this.unit = Objects.requireNonNull(newUnit, "New unit cannot be null");
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return (MutSelf) this;
  }
}
