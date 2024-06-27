package edu.wpi.first.units.angularacceleration;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.angularvelocity.AngularVelocity;
import edu.wpi.first.units.angularvelocity.MutableAngularVelocity;
import edu.wpi.first.units.dimensionless.Dimensionless;
import edu.wpi.first.units.time.Time;

public class MutableAngularAcceleration extends AngularAcceleration {

  /**
   * Creates a new mutable measure that is a copy of the given one.
   *
   * @param measure the measure to create a mutable copy of
   * @return a new mutable measure with an initial state equal to the given
   *         measure
   */
  public static MutableAngularAcceleration mutable(AngularAcceleration measure) {
    return new MutableAngularAcceleration(measure.magnitude(), measure.baseUnitMagnitude(),
        measure.unit());
  }

  /**
   * Creates a new mutable measure with a magnitude of 0 in the given unit.
   *
   * @param unit the unit of measure
   * @return a new mutable measure
   */
  public static MutableAngularAcceleration zero(AngularAccelerationUnit unit) {
    return mutable(unit.zero());
  }

  /**
   * Creates a new mutable measure in the given unit with a magnitude equal to
   * the
   * given one in base
   * units.
   *
   * @param baseUnitMagnitude the magnitude of the measure, in terms of the base
   *                          unit of measure
   * @param unit              the unit of measure
   * @return a new mutable measure
   */
  public static MutableAngularAcceleration ofBaseUnits(double baseUnitMagnitude, AngularAccelerationUnit unit) {
    return new MutableAngularAcceleration(unit.fromBaseUnits(baseUnitMagnitude),
        baseUnitMagnitude, unit);
  }

  /**
   * Creates a new mutable measure in the given unit with a magnitude in terms
   * of
   * that unit.
   *
   * @param <U>               the type of the units of measure
   * @param relativeMagnitude the magnitude of the measure
   * @param unit              the unit of measure
   * @return a new mutable measure
   */
  public static MutableAngularAcceleration ofRelativeUnits(double relativeMagnitude, AngularAccelerationUnit unit) {
    return new MutableAngularAcceleration(relativeMagnitude, unit.toBaseUnits(relativeMagnitude), unit);
  }

  private MutableAngularAcceleration(double initialMagnitude, double baseUnitMagnitude, AngularAccelerationUnit unit) {
    super(initialMagnitude, baseUnitMagnitude, unit);
  }

  // UNSAFE

  /**
   * Sets the new magnitude of the measurement. The magnitude must be in terms
   * of
   * the {@link
   * #unit()}.
   *
   * @param magnitude the new magnitude of the measurement
   */
  public void mut_setMagnitude(double magnitude) {
    m_magnitude = magnitude;
    m_baseUnitMagnitude = m_unit.toBaseUnits(magnitude);
  }

  /**
   * Sets the new magnitude of the measurement. The magnitude must be in terms
   * of
   * the base unit of
   * the current unit.
   *
   * @param baseUnitMagnitude the new magnitude of the measurement
   */
  public void mut_setBaseUnitMagnitude(double baseUnitMagnitude) {
    m_baseUnitMagnitude = baseUnitMagnitude;
    m_magnitude = m_unit.fromBaseUnits(baseUnitMagnitude);
  }

  /**
   * Overwrites the state of this measure and replaces it with values from the
   * given one.
   *
   * @param other the other measure to copy values from
   * @return this measure
   */
  public MutableAngularAcceleration mut_replace(AngularAcceleration other) {
    m_magnitude = other.magnitude();
    m_baseUnitMagnitude = other.baseUnitMagnitude();
    m_unit = other.unit();
    return this;
  }

  /**
   * Overwrites the state of this measure with new values.
   *
   * @param magnitude the new magnitude in terms of the new unit
   * @param unit      the new unit
   * @return this measure
   */
  public MutableAngularAcceleration mut_replace(double magnitude, AngularAccelerationUnit unit) {
    this.m_magnitude = magnitude;
    this.m_baseUnitMagnitude = unit.toBaseUnits(magnitude);
    this.m_unit = unit;
    return this;
  }

  /**
   * Increments (or decrements)
   * the current magnitude of the measure by the given value. The value
   * must be in terms
   * of the current {@link #unit() unit}.
   *
   * @param raw the raw value to increment by
   * @return the measure
   */
  public MutableAngularAcceleration mut_increment(double raw) {
    this.m_magnitude += raw;
    this.m_baseUnitMagnitude += m_unit.toBaseUnits(raw);
    return this;
  }

  /**
   * Increments (or decrements) the current magnitude
   * of the measure by the amount of the given measure.
   *
   * @param other the measure whose value should be added to this one
   * @return the measure
   */
  public MutableAngularAcceleration mut_increment(AngularAcceleration other) {
    m_baseUnitMagnitude += other.baseUnitMagnitude();
    // can't naively use m_magnitude += other.in(m_unit) because the units may
    // not
    // be scalar multiples (eg adding 0C to 100K should result in 373.15K, not
    // 100K)
    m_magnitude = m_unit.fromBaseUnits(m_baseUnitMagnitude);
    return this;
  }

  // Math

  /**
   * Adds another measurement to this one. This will mutate the object instead of
   * generating a new
   * measurement object.
   *
   * @param other the measurement to add
   * @return this measure
   */
  public MutableAngularAcceleration mut_plus(AngularAcceleration other) {
    return mut_plus(other.magnitude(), other.unit());
  }

  /**
   * Adds another measurement to this one. This will mutate the object instead
   * of
   * generating a new
   * measurement object. This is a denormalized version of
   * {@link #mut_plus(Measure)} to avoid
   * having to wrap raw numbers in a {@code Measure} object and pay for an
   * object
   * allocation.
   *
   * @param magnitude the magnitude of the other measurement.
   * @param unit      the unit of the other measurement
   * @return this measure
   */
  public MutableAngularAcceleration mut_plus(double magnitude, AngularAccelerationUnit unit) {
    mut_setBaseUnitMagnitude(m_baseUnitMagnitude + unit.toBaseUnits(magnitude));
    return this;
  }

  /**
   * Subtracts another measurement to this one. This will mutate the object
   * instead of generating a
   * new measurement object.
   *
   * @param other the measurement to add
   * @return this measure
   */
  public MutableAngularAcceleration mut_minus(AngularAcceleration other) {
    return mut_minus(other.magnitude(), other.unit());
  }

  /**
   * Subtracts another measurement to this one. This will mutate the object
   * instead of generating a
   * new measurement object. This is a denormalized version of
   * {@link #mut_minus(Measure)} to avoid
   * having to wrap raw numbers in a {@code Measure} object and pay for an
   * object
   * allocation.
   *
   * @param magnitude the magnitude of the other measurement.
   * @param unit      the unit of the other measurement
   * @return this measure
   */
  public MutableAngularAcceleration mut_minus(double magnitude, AngularAccelerationUnit unit) {
    return mut_plus(-magnitude, unit);
  }

  /**
   * Multiplies this measurement by some constant value. This will mutate the
   * object instead of
   * generating a new measurement object.
   *
   * @param multiplier the multiplier to scale the measurement by
   * @return this measure
   */
  public MutableAngularAcceleration mut_times(double multiplier) {
    mut_setBaseUnitMagnitude(m_baseUnitMagnitude * multiplier);
    return this;
  }

  /**
   * Multiplies this measurement by some constant value. This will mutate the
   * object instead of
   * generating a new measurement object.
   *
   * @param multiplier the multiplier to scale the measurement by
   * @return this measure
   */
  public MutableAngularAcceleration mut_times(Dimensionless multiplier) {
    return mut_times(multiplier.baseUnitMagnitude());
  }

  /**
   * Divides this measurement by some constant value. This will mutate the
   * object
   * instead of
   * generating a new measurement object.
   *
   * @param divisor the divisor to scale the measurement by
   * @return this measure
   */
  public MutableAngularAcceleration mut_divide(double divisor) {
    mut_setBaseUnitMagnitude(m_baseUnitMagnitude / divisor);
    return this;
  }

  /**
   * Divides this measurement by some constant value. This will mutate the
   * object
   * instead of
   * generating a new measurement object.
   *
   * @param divisor the divisor to scale the measurement by
   * @return this measure
   */
  public MutableAngularAcceleration mut_divide(Dimensionless divisor) {
    return mut_divide(divisor.baseUnitMagnitude());
  }

 

}