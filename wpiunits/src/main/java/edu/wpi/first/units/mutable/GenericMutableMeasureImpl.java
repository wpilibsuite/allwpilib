package edu.wpi.first.units.mutable;

import edu.wpi.first.units.ImmutableMeasure;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Unit;
import edu.wpi.first.units.measure.Dimensionless;

public final class GenericMutableMeasureImpl<U extends Unit>
    extends MutableMeasureBase<U, Measure<U>, GenericMutableMeasureImpl<U>> {
  public GenericMutableMeasureImpl(double initialValue, double baseUnitMagnitude, U unit) {
    super(initialValue, baseUnitMagnitude, unit);
  }

  public GenericMutableMeasureImpl(double initialValue, U unit) {
    this(initialValue, unit.toBaseUnits(initialValue), unit);
  }

  @Override
  public Measure<U> copy() {
    return ImmutableMeasure.ofBaseUnits(baseUnitMagnitude, unit);
  }

  @Override
  public MutableMeasure<U, ?, ?> mutableCopy() {
    return new GenericMutableMeasureImpl<>(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public Measure<U> unaryMinus() {
    return ImmutableMeasure.ofBaseUnits(0 - baseUnitMagnitude, unit);
  }

  @Override
  public Measure<U> plus(Measure<? extends U> other) {
    return ImmutableMeasure.ofBaseUnits(baseUnitMagnitude + other.baseUnitMagnitude(), unit);
  }

  @Override
  public Measure<U> minus(Measure<? extends U> other) {
    return ImmutableMeasure.ofBaseUnits(baseUnitMagnitude - other.baseUnitMagnitude(), unit);
  }

  @Override
  public Measure<U> times(double multiplier) {
    return ImmutableMeasure.ofBaseUnits(baseUnitMagnitude * multiplier, unit);
  }

  @Override
  public Measure<U> times(Dimensionless multiplier) {
    return ImmutableMeasure.ofBaseUnits(baseUnitMagnitude * multiplier.baseUnitMagnitude(), unit);
  }

  @Override
  public Measure<U> divide(double divisor) {
    return ImmutableMeasure.ofBaseUnits(baseUnitMagnitude / divisor, unit);
  }

  @Override
  public Measure<U> divide(Dimensionless divisor) {
    return ImmutableMeasure.ofBaseUnits(baseUnitMagnitude / divisor.baseUnitMagnitude(), unit);
  }
}
