// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Value;

import java.util.function.DoubleFunction;

public class MathHelper<U extends Unit, M extends Measure<U>> {
  private final DoubleFunction<M> constructor;

  /**
   * Creates a new unit math helper object.
   *
   * @param constructor the function to use to create new instances of the measure type. This should
   *     accept values in terms of base units.
   */
  public MathHelper(DoubleFunction<M> constructor) {
    this.constructor = constructor;
  }

  public M zero() {
    return constructor.apply(0);
  }

  public M negate(M measure) {
    // 0 minus to avoid negative zero
    return constructor.apply(0 - measure.baseUnitMagnitude());
  }

  public M add(M first, M second) {
    return constructor.apply(first.baseUnitMagnitude() + second.baseUnitMagnitude());
  }

  public M minus(M first, M second) {
    return constructor.apply(first.baseUnitMagnitude() - second.baseUnitMagnitude());
  }

  public M divide(M measure, double divisor) {
    return constructor.apply(measure.baseUnitMagnitude() / divisor);
  }

  public M divide(M measure, Dimensionless divisor) {
    return constructor.apply(measure.baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  public Dimensionless divide(M dividend, M divisor) {
    return divide(dividend, divisor, Value::of);
  }

  public <VU extends Unit, VM extends Measure<VU>> VM divide(
      M dividend, Time divisor, DoubleFunction<? extends VM> ctor) {
    return ctor.apply(dividend.baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  public <ResultUnit extends Unit, Result extends Measure<ResultUnit>> Result divide(
      M dividend, Measure<?> divisor, DoubleFunction<? extends Result> ctor) {
    return ctor.apply(dividend.baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  public <ResultUnit extends Unit, Result extends Measure<ResultUnit>> Result multiply(
      M first, Measure<?> second, DoubleFunction<? extends Result> ctor) {
    return ctor.apply(first.baseUnitMagnitude() * second.baseUnitMagnitude());
  }

  public M multiply(M measure, double scalar) {
    return constructor.apply(measure.baseUnitMagnitude() * scalar);
  }

  public M multiply(M measure, Dimensionless scalar) {
    return constructor.apply(measure.baseUnitMagnitude() * scalar.baseUnitMagnitude());
  }
}
