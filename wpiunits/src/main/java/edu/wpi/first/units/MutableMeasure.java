// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.measure.Dimensionless;

/**
 * A mutable measurement can be used to keep a single object allocation and reference whose state is
 * mutated or changed as it is used. This is helpful for optimizing memory usage to keep garbage
 * collection time - and its associated loop overruns - to a minimum.
 *
 * @param <U> The dimension of measurement.
 * @param <Base> The base measure type.
 * @param <MutSelf> The self type. This MUST inherit from the base measure type.
 */
public interface MutableMeasure<
        U extends Unit, Base extends Measure<U>, MutSelf extends MutableMeasure<U, Base, MutSelf>>
    extends Measure<U> {
  /**
   * Overwrites the state of this measure with new values.
   *
   * @param magnitude the new magnitude in terms of the new unit
   * @param newUnit the new unit
   * @return this measure
   */
  MutSelf mut_replace(double magnitude, U newUnit);

  /**
   * Overwrites the state of this measure and replaces it completely with values from the given one.
   * The magnitude, base unit magnitude, and unit will all be copied. This is functionally the same
   * as calling {@code other.mutableCopy()}, but copying to a pre-existing mutable measure instead
   * of instantiating a new one.
   *
   * @param other the other measure to copy values from
   * @return this measure
   */
  default MutSelf mut_replace(Base other) {
    return mut_replace(other.magnitude(), other.unit());
  }

  @Override
  Base copy();

  /**
   * Sets the new magnitude of the measurement. The magnitude must be in terms of the {@link
   * #unit()}.
   *
   * @param magnitude the new magnitude of the measurement
   * @return this mutable measure
   */
  default MutSelf mut_setMagnitude(double magnitude) {
    return mut_replace(magnitude, unit());
  }

  /**
   * Sets the new magnitude of the measurement. The magnitude must be in terms of the base unit of
   * the current unit.
   *
   * @param baseUnitMagnitude the new magnitude of the measurement
   * @return this mutable measure
   */
  default MutSelf mut_setBaseUnitMagnitude(double baseUnitMagnitude) {
    return mut_replace(unit().fromBaseUnits(baseUnitMagnitude), unit());
  }

  /**
   * Increments the current magnitude of the measure by the given value. The value must be in terms
   * of the current {@link #unit() unit}.
   *
   * @param raw the raw value to accumulate by
   * @return the measure
   */
  default MutSelf mut_acc(double raw) {
    return mut_setBaseUnitMagnitude(magnitude() + raw);
  }

  /**
   * Increments the current magnitude of the measure by the amount of the given measure.
   *
   * @param other the measure whose value should be added to this one
   * @return this measure
   */
  default MutSelf mut_acc(Base other) {
    return mut_setMagnitude(magnitude() + unit().fromBaseUnits(other.baseUnitMagnitude()));
  }

  /**
   * Adds another measurement to this one. This will mutate the object instead of generating a new
   * measurement object.
   *
   * @param other the measurement to add
   * @return this measure
   */
  default MutSelf mut_plus(Base other) {
    return mut_acc(other);
  }

  /**
   * Adds another measurement to this one. This will mutate the object instead of generating a new
   * measurement object. This is a denormalized version of {@link #mut_plus(Measure)} to avoid
   * having to wrap raw numbers in a {@code Measure} object and pay for an object allocation.
   *
   * @param magnitude the magnitude of the other measurement.
   * @param otherUnit the unit of the other measurement
   * @return this measure
   */
  default MutSelf mut_plus(double magnitude, U otherUnit) {
    return mut_setBaseUnitMagnitude(magnitude() + otherUnit.toBaseUnits(magnitude));
  }

  /**
   * Subtracts another measurement to this one. This will mutate the object instead of generating a
   * new measurement object.
   *
   * @param other the measurement to subtract from this one
   * @return this measure
   */
  default MutSelf mut_minus(Base other) {
    return mut_setBaseUnitMagnitude(baseUnitMagnitude() - other.baseUnitMagnitude());
  }

  /**
   * Subtracts another measurement from this one. This will mutate the object instead of generating
   * a new measurement object. This is a denormalized version of {@link #mut_minus(Measure)} to
   * avoid having to wrap raw numbers in a {@code Measure} object and pay for an object allocation.
   *
   * @param magnitude the magnitude of the other measurement.
   * @param otherUnit the unit of the other measurement
   * @return this measure
   */
  default MutSelf mut_minus(double magnitude, U otherUnit) {
    return mut_setBaseUnitMagnitude(baseUnitMagnitude() - otherUnit.toBaseUnits(magnitude));
  }

  /**
   * Multiplies this measurement by some constant value. This will mutate the object instead of
   * generating a new measurement object.
   *
   * @param multiplier the multiplier to scale the measurement by
   * @return this measure
   */
  default MutSelf mut_times(double multiplier) {
    return mut_setBaseUnitMagnitude(baseUnitMagnitude() * multiplier);
  }

  /**
   * Multiplies this measurement by some constant value. This will mutate the object instead of
   * generating a new measurement object.
   *
   * @param multiplier the multiplier to scale the measurement by
   * @return this measure
   */
  default MutSelf mut_times(Dimensionless multiplier) {
    return mut_times(multiplier.baseUnitMagnitude());
  }

  /**
   * Divides this measurement by some constant value. This will mutate the object instead of
   * generating a new measurement object.
   *
   * @param divisor the divisor to scale the measurement by
   * @return this measure
   */
  default MutSelf mut_divide(double divisor) {
    return mut_times(1 / divisor);
  }

  /**
   * Divides this measurement by some constant value. This will mutate the object instead of
   * generating a new measurement object.
   *
   * @param divisor the divisor to scale the measurement by
   * @return this measure
   */
  default MutSelf mut_divide(Dimensionless divisor) {
    return mut_divide(divisor.baseUnitMagnitude());
  }
}
