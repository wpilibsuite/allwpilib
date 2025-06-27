// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Value;

import edu.wpi.first.units.measure.Acceleration;
import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.units.measure.AngularAcceleration;
import edu.wpi.first.units.measure.AngularMomentum;
import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.Dimensionless;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.Energy;
import edu.wpi.first.units.measure.Force;
import edu.wpi.first.units.measure.Frequency;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.units.measure.LinearMomentum;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.Mass;
import edu.wpi.first.units.measure.MomentOfInertia;
import edu.wpi.first.units.measure.Mult;
import edu.wpi.first.units.measure.Per;
import edu.wpi.first.units.measure.Power;
import edu.wpi.first.units.measure.Resistance;
import edu.wpi.first.units.measure.Temperature;
import edu.wpi.first.units.measure.Time;
import edu.wpi.first.units.measure.Torque;
import edu.wpi.first.units.measure.Velocity;
import edu.wpi.first.units.measure.Voltage;

/**
 * A measure holds the magnitude and unit of some dimension, such as distance, time, or speed. Two
 * measures with the same <i>unit</i> and <i>magnitude</i> are effectively equivalent objects.
 *
 * @param <U> the unit type of the measure
 */
public interface Measure<U extends Unit> extends Comparable<Measure<U>> {
  /**
   * The threshold for two measures to be considered equivalent if converted to the same unit. This
   * is only needed due to floating-point error.
   */
  double EQUIVALENCE_THRESHOLD = 1e-12;

  /**
   * Gets the unitless magnitude of this measure.
   *
   * @return the magnitude in terms of {@link #unit() the unit}.
   */
  double magnitude();

  /**
   * Gets the magnitude of this measure in terms of the base unit. If the unit is the base unit for
   * its system of measure, then the value will be equivalent to {@link #magnitude()}.
   *
   * @return the magnitude in terms of the base unit
   */
  double baseUnitMagnitude();

  /**
   * Gets the units of this measure.
   *
   * @return the unit
   */
  U unit();

  /**
   * Converts this measure to a measure with a different unit of the same type, eg minutes to
   * seconds. Converting to the same unit is equivalent to calling {@link #magnitude()}.
   *
   * <pre>
   *   Meters.of(12).in(Feet) // 39.3701
   *   Seconds.of(15).in(Minutes) // 0.25
   * </pre>
   *
   * @param unit the unit to convert this measure to
   * @return the value of this measure in the given unit
   */
  default double in(U unit) {
    if (this.unit().equals(unit)) {
      return magnitude();
    } else {
      return unit.fromBaseUnits(baseUnitMagnitude());
    }
  }

  /**
   * A convenience method to get the base unit of the measurement. Equivalent to {@code
   * unit().getBaseUnit()}.
   *
   * @return the base unit of measure.
   */
  @SuppressWarnings("unchecked")
  default U baseUnit() {
    return (U) unit().getBaseUnit();
  }

  /**
   * Absolute value of measure.
   *
   * @param unit unit to use
   * @return the absolute value of this measure in the given unit
   */
  default double abs(U unit) {
    return Math.abs(this.in(unit));
  }

  /**
   * Take the sign of another measure.
   *
   * @param other measure from which to take sign
   * @param unit unit to use
   * @return the value of the measure in the given unit with the sign of the provided measure
   */
  default double copySign(Measure<U> other, U unit) {
    return Math.copySign(this.in(unit), other.in(unit));
  }

  /**
   * Returns an immutable copy of this measure. The copy can be used freely and is guaranteed never
   * to change.
   *
   * @return the copied measure
   */
  Measure<U> copy();

  /**
   * Returns a mutable copy of this measure. It will be initialized to the current state of this
   * measure, but can be changed over time without needing to allocate new measurement objects.
   *
   * @return the copied measure
   */
  MutableMeasure<U, ?, ?> mutableCopy();

  /**
   * Returns a measure equivalent to this one equal to zero minus its current value. For non-linear
   * unit types like temperature, the zero point is treated as the zero value of the base unit (eg
   * Kelvin). In effect, this means code like {@code Celsius.of(10).unaryMinus()} returns a value
   * equivalent to -10 Kelvin, and <i>not</i> -10° Celsius.
   *
   * @return a measure equal to zero minus this measure
   */
  Measure<U> unaryMinus();

  /**
   * Returns a measure equivalent to this one equal to zero minus its current value. For non-linear
   * unit types like temperature, the zero point is treated as the zero value of the base unit (eg
   * Kelvin). In effect, this means code like {@code Celsius.of(10).negate()} returns a value
   * equivalent to -10 Kelvin, and <i>not</i> -10° Celsius.
   *
   * @return a measure equal to zero minus this measure
   * @deprecated use unaryMinus() instead. This was renamed for consistency with other WPILib
   *     classes like Rotation2d
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<U> negate() {
    return unaryMinus();
  }

  /**
   * Adds another measure of the same unit type to this one.
   *
   * @param other the measurement to add
   * @return a measure of the sum of both measures
   */
  Measure<U> plus(Measure<? extends U> other);

  /**
   * Subtracts another measure of the same unit type from this one.
   *
   * @param other the measurement to subtract
   * @return a measure of the difference between the measures
   */
  Measure<U> minus(Measure<? extends U> other);

  /**
   * Multiplies this measure by a scalar unitless multiplier.
   *
   * @param multiplier the scalar multiplication factor
   * @return the scaled result
   */
  Measure<U> times(double multiplier);

  /**
   * Multiplies this measure by a scalar dimensionless multiplier.
   *
   * @param multiplier the scalar multiplication factor
   * @return the scaled result
   */
  Measure<U> times(Dimensionless multiplier);

  /**
   * Generates a new measure that is equal to this measure multiplied by another. Some dimensional
   * analysis is performed to reduce the units down somewhat; for example, multiplying a {@code
   * Measure<Time>} by a {@code Measure<Velocity<Distance>>} will return just a {@code
   * Measure<Distance>} instead of the naive {@code Measure<Mult<Time, Velocity<Distance>>}. This is
   * not guaranteed to perform perfect dimensional analysis.
   *
   * @param multiplier the unit to multiply by
   * @return the multiplicative unit
   */
  default Measure<?> times(Measure<?> multiplier) {
    final double baseUnitResult = baseUnitMagnitude() * multiplier.baseUnitMagnitude();

    // First try to eliminate any common units
    if (unit() instanceof PerUnit<?, ?> ratio
        && multiplier.baseUnit().equals(ratio.denominator().getBaseUnit())) {
      // Per<N, D> * D -> yield N
      // Case 1: denominator of the Per cancels out, return with just the units of the numerator
      return ratio.numerator().ofBaseUnits(baseUnitResult);
    } else if (multiplier.baseUnit() instanceof PerUnit<?, ?> ratio
        && baseUnit().equals(ratio.denominator().getBaseUnit())) {
      // D * Per<N, D) -> yield N
      // Case 2: Same as Case 1, just flipped between this and the multiplier
      return ratio.numerator().ofBaseUnits(baseUnitResult);
    } else if (unit() instanceof PerUnit<?, ?> per
        && multiplier.unit() instanceof PerUnit<?, ?> otherPer
        && per.denominator().getBaseUnit().equals(otherPer.numerator().getBaseUnit())
        && per.numerator().getBaseUnit().equals(otherPer.denominator().getBaseUnit())) {
      // multiplying eg meters per second * milliseconds per foot
      // return a scalar
      return Value.of(baseUnitResult);
    }

    // No common units to eliminate, is one of them dimensionless?
    // Note that this must come *after* the multiplier cases, otherwise
    // Per<U, Dimensionless> * Dimensionless will not return a U
    if (multiplier.unit() instanceof DimensionlessUnit) {
      // scalar multiplication of this
      return times(multiplier.baseUnitMagnitude());
    } else if (unit() instanceof DimensionlessUnit) {
      // scalar multiplication of multiplier
      return multiplier.times(baseUnitMagnitude());
    }

    if (multiplier instanceof Acceleration<?> acceleration) {
      return times(acceleration);
    } else if (multiplier instanceof Angle angle) {
      return times(angle);
    } else if (multiplier instanceof AngularAcceleration angularAcceleration) {
      return times(angularAcceleration);
    } else if (multiplier instanceof AngularMomentum angularMomentum) {
      return times(angularMomentum);
    } else if (multiplier instanceof AngularVelocity angularVelocity) {
      return times(angularVelocity);
    } else if (multiplier instanceof Current current) {
      return times(current);
    } else if (multiplier instanceof Dimensionless dimensionless) {
      // n.b. this case should already be covered
      return times(dimensionless);
    } else if (multiplier instanceof Distance distance) {
      return times(distance);
    } else if (multiplier instanceof Energy energy) {
      return times(energy);
    } else if (multiplier instanceof Force force) {
      return times(force);
    } else if (multiplier instanceof Frequency frequency) {
      return times(frequency);
    } else if (multiplier instanceof LinearAcceleration linearAcceleration) {
      return times(linearAcceleration);
    } else if (multiplier instanceof LinearVelocity linearVelocity) {
      return times(linearVelocity);
    } else if (multiplier instanceof Mass mass) {
      return times(mass);
    } else if (multiplier instanceof MomentOfInertia momentOfInertia) {
      return times(momentOfInertia);
    } else if (multiplier instanceof Mult<?, ?> mult) {
      return times(mult);
    } else if (multiplier instanceof Per<?, ?> per) {
      return times(per);
    } else if (multiplier instanceof Power power) {
      return times(power);
    } else if (multiplier instanceof Temperature temperature) {
      return times(temperature);
    } else if (multiplier instanceof Time time) {
      return times(time);
    } else if (multiplier instanceof Torque torque) {
      return times(torque);
    } else if (multiplier instanceof Velocity<?> velocity) {
      return times(velocity);
    } else if (multiplier instanceof Voltage voltage) {
      return times(voltage);
    } else if (multiplier instanceof Resistance resistance) {
      return times(resistance);
    } else {
      // Dimensional analysis fallthrough or a generic input measure type
      // Do a basic unit multiplication
      return MultUnit.combine(unit(), multiplier.unit()).ofBaseUnits(baseUnitResult);
    }
  }

  /**
   * Multiplies this measure by an acceleration and returns the resulting measure in the most
   * appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Acceleration<?> multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by an angle and returns the resulting measure in the most appropriate
   * unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Angle multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by an angular acceleration and returns the resulting measure in the
   * most appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(AngularAcceleration multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by an angular momentum and returns the resulting measure in the most
   * appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(AngularMomentum multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by an angular velocity and returns the resulting measure in the most
   * appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(AngularVelocity multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by an electric current and returns the resulting measure in the most
   * appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Current multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a distance and returns the resulting measure in the most appropriate
   * unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Distance multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by an energy and returns the resulting measure in the most appropriate
   * unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Energy multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a force and returns the resulting measure in the most appropriate
   * unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Force multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a frequency and returns the resulting measure in the most
   * appropriate unit. This often - but not always - means implementations return a variation of a
   * {@link Per} measure.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Frequency multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a linear acceleration and returns the resulting measure in the most
   * appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(LinearAcceleration multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a linear momentum and returns the resulting measure in the most
   * appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(LinearMomentum multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a linear velocity and returns the resulting measure in the most
   * appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(LinearVelocity multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a mass and returns the resulting measure in the most appropriate
   * unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Mass multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a moment of intertia and returns the resulting measure in the most
   * appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(MomentOfInertia multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a generic multiplied measure and returns the resulting measure in
   * the most appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Mult<?, ?> multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a power and returns the resulting measure in the most appropriate
   * unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Power multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a generic ratio measurement and returns the resulting measure in the
   * most appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Per<?, ?> multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a temperature and returns the resulting measure in the most
   * appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Temperature multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a time and returns the resulting measure in the most appropriate
   * unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Time multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a torque and returns the resulting measure in the most appropriate
   * unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Torque multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a generic velocity and returns the resulting measure in the most
   * appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Velocity<?> multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a voltage and returns the resulting measure in the most appropriate
   * unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Voltage multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a resistance and returns the resulting measure in the most
   * appropriate unit.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Measure<?> times(Resistance multiplier) {
    return MultUnit.combine(unit(), multiplier.unit())
        .ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by a conversion factor, returning the converted measurement. Unlike
   * {@link #times(Per)}, this allows for basic unit cancellation to return measurements of a known
   * dimension.
   *
   * @param conversionFactor the conversion factor by which to multiply
   * @param <Other> the unit type to convert to
   * @param <M> the concrete return unit type. <strong>Note: the conversion factor's numerator unit
   *     must return instances of this type from {@link Unit#ofBaseUnits(double)}}</strong>
   * @return the converted result
   */
  @SuppressWarnings("unchecked")
  default <Other extends Unit, M extends Measure<Other>> M timesConversionFactor(
      Measure<? extends PerUnit<Other, U>> conversionFactor) {
    return (M)
        conversionFactor
            .unit()
            .getBaseUnit()
            .numerator()
            .ofBaseUnits(baseUnitMagnitude() * conversionFactor.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by another measurement of the inverse unit type (eg Time multiplied by
   * Frequency) and returns the resulting dimensionless measure.
   *
   * @param multiplier the measurement to multiply by.
   * @return the multiplication result
   */
  default Dimensionless timesInverse(
      Measure<? extends PerUnit<DimensionlessUnit, ? extends U>> multiplier) {
    return Value.ofBaseUnits(baseUnitMagnitude() * multiplier.baseUnitMagnitude());
  }

  /**
   * Multiplies this measure by another measurement of something over the unit type (eg Time
   * multiplied by LinearVelocity) and returns the resulting measure of the ratio's dividend unit.
   *
   * @param ratio the measurement to multiply by.
   * @param <Other> other unit that the results are in terms of
   * @return the multiplication result
   */
  default <Other extends Unit> Measure<Other> timesRatio(
      Measure<? extends PerUnit<? extends Other, U>> ratio) {
    return ImmutableMeasure.ofBaseUnits(
        baseUnitMagnitude() * ratio.baseUnitMagnitude(), ratio.unit().numerator());
  }

  /**
   * Divides this measure by a unitless scalar and returns the result.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  Measure<U> div(double divisor);

  /**
   * Divides this measure by a dimensionless scalar and returns the result.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  Measure<U> div(Dimensionless divisor);

  /**
   * Divides this measurement by another measure and performs some dimensional analysis to reduce
   * the units.
   *
   * @param divisor the unit to divide by
   * @return the resulting measure
   */
  default Measure<?> div(Measure<?> divisor) {
    final double baseUnitResult = baseUnitMagnitude() / divisor.baseUnitMagnitude();

    if (unit().getBaseUnit().equals(divisor.unit().getBaseUnit())) {
      // These are the same unit, return a dimensionless
      return Value.ofBaseUnits(baseUnitResult);
    }

    if (divisor instanceof Dimensionless) {
      // Dividing by a dimensionless, do a scalar division
      return unit().ofBaseUnits(baseUnitResult);
    }

    if (unit() instanceof DimensionlessUnit) {
      // Numerator is a dimensionless
      if (divisor.unit() instanceof PerUnit<?, ?> ratio) {
        // Dividing by a ratio, return its reciprocal scaled by this
        return ratio.reciprocal().ofBaseUnits(baseUnitResult);
      }
      if (divisor.unit() instanceof PerUnit<?, ?> ratio) {
        // Dividing by a Per<Time, U>, return its reciprocal velocity scaled by this
        // Note: Per<Time, U>.reciprocal() is coded to return a Velocity<U>
        return ratio.reciprocal().ofBaseUnits(baseUnitResult);
      }
    }

    if (divisor.unit() instanceof PerUnit<?, ?> ratio
        && ratio.numerator().getBaseUnit().equals(baseUnit())) {
      // Numerator of the ratio cancels out
      // N / (N / D) -> N * D / N -> D
      // Note: all Velocity and Acceleration units inherit from PerUnit
      return ratio.denominator().ofBaseUnits(baseUnitResult);
    }

    if (divisor.unit() instanceof TimeUnit time) {
      // Dividing by a time, return a Velocity
      return VelocityUnit.combine(unit(), time).ofBaseUnits(baseUnitResult);
    }

    if (divisor instanceof Acceleration<?> acceleration) {
      return div(acceleration);
    } else if (divisor instanceof Angle angle) {
      return div(angle);
    } else if (divisor instanceof AngularAcceleration angularAcceleration) {
      return div(angularAcceleration);
    } else if (divisor instanceof AngularMomentum angularMomentum) {
      return div(angularMomentum);
    } else if (divisor instanceof AngularVelocity angularVelocity) {
      return div(angularVelocity);
    } else if (divisor instanceof Current current) {
      return div(current);
    } else if (divisor instanceof Dimensionless dimensionless) {
      // n.b. this case should already be covered
      return div(dimensionless);
    } else if (divisor instanceof Distance distance) {
      return div(distance);
    } else if (divisor instanceof Energy energy) {
      return div(energy);
    } else if (divisor instanceof Force force) {
      return div(force);
    } else if (divisor instanceof Frequency frequency) {
      return div(frequency);
    } else if (divisor instanceof LinearAcceleration linearAcceleration) {
      return div(linearAcceleration);
    } else if (divisor instanceof LinearVelocity linearVelocity) {
      return div(linearVelocity);
    } else if (divisor instanceof Mass mass) {
      return div(mass);
    } else if (divisor instanceof MomentOfInertia momentOfInertia) {
      return div(momentOfInertia);
    } else if (divisor instanceof Mult<?, ?> mult) {
      return div(mult);
    } else if (divisor instanceof Per<?, ?> per) {
      return div(per);
    } else if (divisor instanceof Power power) {
      return div(power);
    } else if (divisor instanceof Temperature temperature) {
      return div(temperature);
    } else if (divisor instanceof Time time) {
      return div(time);
    } else if (divisor instanceof Torque torque) {
      return div(torque);
    } else if (divisor instanceof Velocity<?> velocity) {
      return div(velocity);
    } else if (divisor instanceof Voltage voltage) {
      return div(voltage);
    } else if (divisor instanceof Resistance resistance) {
      return div(resistance);
    } else {
      // Dimensional analysis fallthrough or a generic input measure type
      // Do a basic unit multiplication
      return PerUnit.combine(unit(), divisor.unit()).ofBaseUnits(baseUnitResult);
    }
  }

  /**
   * Divides this measure by a generic acceleration and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Acceleration<?> divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by an angle and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Angle divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by an angular acceleration and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(AngularAcceleration divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by an angular momentum and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(AngularMomentum divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by an angular velocity and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(AngularVelocity divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by an electric current and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Current divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a distance and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Distance divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by an energy and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Energy divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a force and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Force divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a frequency and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Frequency divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a linear acceleration and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(LinearAcceleration divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a linear momentum and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(LinearMomentum divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a linear velocity and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(LinearVelocity divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a mass and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Mass divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a moment of inertia and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(MomentOfInertia divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a generic multiplication and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Mult<?, ?> divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a power and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Power divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a generic ratio and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Per<?, ?> divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a temperature and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Temperature divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a time and returns the result in the most appropriate unit. This will
   * often - but not always - result in a {@link Per} type like {@link LinearVelocity} or {@link
   * Acceleration}.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Time divisor) {
    return VelocityUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a torque and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Torque divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a generic velocity and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Velocity<?> divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a voltage and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Voltage divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by a resistance and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   */
  default Measure<?> div(Resistance divisor) {
    return PerUnit.combine(unit(), divisor.unit())
        .ofBaseUnits(baseUnitMagnitude() / divisor.baseUnitMagnitude());
  }

  /**
   * Divides this measure by an acceleration unit and returns the result in the most appropriate
   * unit. This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(AccelerationUnit<?> divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by an angle unit and returns the result in the most appropriate unit. This
   * is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(AngleUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by an angular acceleration unit and returns the result in the most
   * appropriate unit. This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(AngularAccelerationUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by an angular momentum unit and returns the result in the most appropriate
   * unit. This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(AngularMomentumUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by an angular velocity unit and returns the result in the most appropriate
   * unit. This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(AngularVelocityUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a unit of electric current and returns the result in the most
   * appropriate unit. This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(CurrentUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a distance unit and returns the result in the most appropriate unit.
   * This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(DistanceUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by an energy unit and returns the result in the most appropriate unit.
   * This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(EnergyUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a force unit and returns the result in the most appropriate unit. This
   * is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(ForceUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a frequency unit and returns the result in the most appropriate unit.
   * This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(FrequencyUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a linear acceleration unit and returns the result in the most
   * appropriate unit. This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(LinearAccelerationUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a linear momentum unit and returns the result in the most appropriate
   * unit. This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(LinearMomentumUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a linear velocity unit and returns the result in the most appropriate
   * unit. This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(LinearVelocityUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a mass unit and returns the result in the most appropriate unit. This
   * is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(MassUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a moment of inertia unit and returns the result in the most appropriate
   * unit. This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(MomentOfInertiaUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a generic compound unit and returns the result in the most appropriate
   * unit. This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(MultUnit<?, ?> divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a power unit and returns the result in the most appropriate unit. This
   * is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(PowerUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a generic ratio unit and returns the result in the most appropriate
   * unit. This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(PerUnit<?, ?> divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a temperature unit and returns the result in the most appropriate unit.
   * This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(TemperatureUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a time period and returns the result in the most appropriate unit. This
   * is equivalent to {@code div(period.of(1))}.
   *
   * @param period the time period measurement to divide by.
   * @return the division result
   */
  default Measure<?> per(TimeUnit period) {
    return div(period.of(1));
  }

  /**
   * Divides this measure by a torque unit and returns the result in the most appropriate unit. This
   * is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(TorqueUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a velocity unit and returns the result in the most appropriate unit.
   * This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(VelocityUnit<?> divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a voltage unit and returns the result in the most appropriate unit.
   * This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(VoltageUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a resistance unit and returns the result in the most appropriate unit.
   * This is equivalent to {@code div(divisorUnit.of(1))}.
   *
   * @param divisorUnit the unit to divide by.
   * @return the division result
   */
  default Measure<?> per(ResistanceUnit divisorUnit) {
    return div(divisorUnit.one());
  }

  /**
   * Divides this measure by a unitless scalar and returns the result.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<U> divide(double divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a dimensionless scalar and returns the result.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<U> divide(Dimensionless divisor) {
    return div(divisor);
  }

  /**
   * Divides this measurement by another measure and performs some dimensional analysis to reduce
   * the units.
   *
   * @param divisor the unit to divide by
   * @return the resulting measure
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Measure<?> divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a generic acceleration and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Acceleration<?> divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by an angle and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Angle divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by an angular acceleration and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(AngularAcceleration divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by an angular momentum and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(AngularMomentum divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by an angular velocity and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(AngularVelocity divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by an electric current and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Current divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a distance and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Distance divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by an energy and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Energy divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a force and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Force divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a frequency and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Frequency divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a linear acceleration and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(LinearAcceleration divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a linear momentum and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(LinearMomentum divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a linear velocity and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(LinearVelocity divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a mass and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Mass divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a moment of inertia and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(MomentOfInertia divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a generic multiplication and returns the result in the most appropriate
   * unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Mult<?, ?> divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a power and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Power divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a generic ratio and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Per<?, ?> divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a temperature and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Temperature divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a time and returns the result in the most appropriate unit. This will
   * often - but not always - result in a {@link Per} type like {@link LinearVelocity} or {@link
   * Acceleration}.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Time divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a torque and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Torque divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a generic velocity and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Velocity<?> divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a voltage and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Voltage divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a resistance and returns the result in the most appropriate unit.
   *
   * @param divisor the measurement to divide by.
   * @return the division result
   * @deprecated use div instead. This was renamed for consistency with other languages like Kotlin
   */
  @Deprecated(since = "2025", forRemoval = true)
  default Measure<?> divide(Resistance divisor) {
    return div(divisor);
  }

  /**
   * Divides this measure by a ratio in terms of this measurement's unit to another unit, returning
   * a measurement in terms of the other unit.
   *
   * @param divisor the measurement to divide by.
   * @param <Other> the other unit that results are in terms of
   * @return the division result
   */
  // eg Dimensionless / (Dimensionless / Time) -> Time
  default <Other extends Unit> Measure<Other> divideRatio(
      Measure<? extends PerUnit<? extends U, Other>> divisor) {
    return ImmutableMeasure.ofBaseUnits(
        baseUnitMagnitude() / divisor.baseUnitMagnitude(), divisor.unit().denominator());
  }

  /**
   * Checks if this measure is near another measure of the same unit. Provide a variance threshold
   * for use for a +/- scalar, such as 0.05 for +/- 5%.
   *
   * <pre>
   *   Inches.of(11).isNear(Inches.of(10), 0.1) // true
   *   Inches.of(12).isNear(Inches.of(10), 0.1) // false
   * </pre>
   *
   * @param other the other measurement to compare against
   * @param varianceThreshold the acceptable variance threshold, in terms of an acceptable +/- error
   *     range multiplier. Checking if a value is within 10% means a value of 0.1 should be passed;
   *     checking if a value is within 1% means a value of 0.01 should be passed, and so on.
   * @return true if this unit is near the other measure, otherwise false
   */
  default boolean isNear(Measure<?> other, double varianceThreshold) {
    if (!this.unit().getBaseUnit().equivalent(other.unit().getBaseUnit())) {
      return false; // Disjoint units, not compatible
    }

    // abs so negative inputs are calculated correctly
    var tolerance = Math.abs(other.baseUnitMagnitude() * varianceThreshold);

    return Math.abs(this.baseUnitMagnitude() - other.baseUnitMagnitude()) <= tolerance;
  }

  /**
   * Checks if this measure is near another measure of the same unit, with a specified tolerance of
   * the same unit.
   *
   * <pre>
   *     Meters.of(1).isNear(Meters.of(1.2), Millimeters.of(300)) // true
   *     Degrees.of(90).isNear(Rotations.of(0.5), Degrees.of(45)) // false
   * </pre>
   *
   * @param other the other measure to compare against.
   * @param tolerance the tolerance allowed in which the two measures are defined as near each
   *     other.
   * @return true if this unit is near the other measure, otherwise false.
   */
  default boolean isNear(Measure<U> other, Measure<U> tolerance) {
    return Math.abs(this.baseUnitMagnitude() - other.baseUnitMagnitude())
        <= Math.abs(tolerance.baseUnitMagnitude());
  }

  /**
   * Checks if this measure is equivalent to another measure of the same unit.
   *
   * @param other the measure to compare to
   * @return true if this measure is equivalent, false otherwise
   */
  default boolean isEquivalent(Measure<?> other) {
    // Return false for disjoint units that aren't compatible
    return this.unit().getBaseUnit().equals(other.unit().getBaseUnit())
        && Math.abs(baseUnitMagnitude() - other.baseUnitMagnitude()) <= EQUIVALENCE_THRESHOLD;
  }

  /** {@inheritDoc} */
  @Override
  default int compareTo(Measure<U> o) {
    return Double.compare(this.baseUnitMagnitude(), o.baseUnitMagnitude());
  }

  /**
   * Checks if this measure is greater than another measure of the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has a greater equivalent magnitude, false otherwise
   */
  default boolean gt(Measure<U> o) {
    return compareTo(o) > 0;
  }

  /**
   * Checks if this measure is greater than or equivalent to another measure of the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has an equal or greater equivalent magnitude, false otherwise
   */
  default boolean gte(Measure<U> o) {
    return compareTo(o) > 0 || isEquivalent(o);
  }

  /**
   * Checks if this measure is less than another measure of the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has a lesser equivalent magnitude, false otherwise
   */
  default boolean lt(Measure<U> o) {
    return compareTo(o) < 0;
  }

  /**
   * Checks if this measure is less than or equivalent to another measure of the same unit.
   *
   * @param o the other measure to compare to
   * @return true if this measure has an equal or lesser equivalent magnitude, false otherwise
   */
  default boolean lte(Measure<U> o) {
    return compareTo(o) < 0 || isEquivalent(o);
  }

  /**
   * Returns the measure with the absolute value closest to positive infinity.
   *
   * @param <U> the type of the units of the measures
   * @param measures the set of measures to compare
   * @return the measure with the greatest positive magnitude, or null if no measures were provided
   */
  @SafeVarargs
  static <U extends Unit> Measure<U> max(Measure<U>... measures) {
    if (measures.length == 0) {
      return null; // nothing to compare
    }

    Measure<U> max = null;
    for (Measure<U> measure : measures) {
      if (max == null || measure.gt(max)) {
        max = measure;
      }
    }

    return max;
  }

  /**
   * Returns the measure with the absolute value closest to negative infinity.
   *
   * @param <U> the type of the units of the measures
   * @param measures the set of measures to compare
   * @return the measure with the greatest negative magnitude
   */
  @SafeVarargs
  static <U extends Unit> Measure<U> min(Measure<U>... measures) {
    if (measures.length == 0) {
      return null; // nothing to compare
    }

    Measure<U> max = null;
    for (Measure<U> measure : measures) {
      if (max == null || measure.lt(max)) {
        max = measure;
      }
    }

    return max;
  }

  /**
   * Returns a string representation of this measurement in a shorthand form. The symbol of the
   * backing unit is used, rather than the full name, and the magnitude is represented in scientific
   * notation.
   *
   * @return the short form representation of this measurement
   */
  default String toShortString() {
    // eg 1.234e+04 V/m (1234 Volt per Meter in long form)
    return String.format("%.3e %s", magnitude(), unit().symbol());
  }

  /**
   * Returns a string representation of this measurement in a longhand form. The name of the backing
   * unit is used, rather than its symbol, and the magnitude is represented in a full string, not
   * scientific notation. (Very large values may be represented in scientific notation, however)
   *
   * @return the long form representation of this measurement
   */
  default String toLongString() {
    // eg 1234 Volt per Meter (1.234e+04 V/m in short form)
    return String.format("%s %s", magnitude(), unit().name());
  }
}
