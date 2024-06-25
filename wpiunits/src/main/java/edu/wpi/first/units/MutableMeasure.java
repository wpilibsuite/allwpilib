// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/**
 * @param <U> The dimension of measurement.
 * @param <Base> The base measure type.
 * @param <MutSelf> The self type. This MUST inherit from the base measure type.
 */
public interface MutableMeasure<
        U extends Unit, Base extends Measure<U>, MutSelf extends MutableMeasure<U, Base, MutSelf>>
    extends Measure<U> {
  MutSelf mut_replace(double magnitude, U newUnit);

  Base copy();

  default MutSelf mut_setMagnitude(double magnitude) {
    return mut_replace(magnitude, unit());
  }

  default MutSelf mut_setBaseUnitMagnitude(double baseUnitMagnitude) {
    return mut_replace(unit().fromBaseUnits(baseUnitMagnitude), unit());
  }

  default MutSelf mut_replace(Base other) {
    return mut_replace(other.magnitude(), other.unit());
  }

  default MutSelf mut_acc(double raw) {
    return mut_setBaseUnitMagnitude(magnitude() + raw);
  }

  default MutSelf mut_acc(Base other) {
    return mut_setMagnitude(magnitude() + unit().fromBaseUnits(other.baseUnitMagnitude()));
  }

  default MutSelf mut_plus(Base other) {
    return mut_acc(other);
  }

  default MutSelf mut_plus(double magnitude, U otherUnit) {
    return mut_setBaseUnitMagnitude(magnitude() + otherUnit.toBaseUnits(magnitude));
  }

  default MutSelf mut_minus(Base other) {
    return mut_setBaseUnitMagnitude(baseUnitMagnitude() - other.baseUnitMagnitude());
  }

  default MutSelf mut_minus(double magnitude, U otherUnit) {
    return mut_setBaseUnitMagnitude(baseUnitMagnitude() - otherUnit.toBaseUnits(magnitude));
  }

  default MutSelf mut_times(double multiplier) {
    return mut_setBaseUnitMagnitude(baseUnitMagnitude() * multiplier);
  }

  default MutSelf mut_times(Dimensionless scalar) {
    return mut_times(scalar.baseUnitMagnitude());
  }

  default MutSelf mut_divide(double divisor) {
    return mut_times(1 / divisor);
  }

  default MutSelf mut_divide(Dimensionless divisor) {
    return mut_divide(divisor.baseUnitMagnitude());
  }
}
