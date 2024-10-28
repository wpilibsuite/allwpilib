// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import java.util.Locale;

/** Contains a set of predefined units. */
public final class Units {
  private Units() {
    // Prevent instantiation
  }

  // Pseudo-classes describing the more common units of measure.

  // Unitless
  /** A dimensionless unit that performs no scaling whatsoever. */
  public static final DimensionlessUnit Value = BaseUnits.Value;

  /**
   * A dimensionless unit equal to to 1/100th of a {@link #Value}. A measurement of {@code
   * Percent.of(42)} would be equivalent to {@code Value.of(0.42)}.
   */
  public static final DimensionlessUnit Percent =
      derive(Value).splitInto(100).named("Percent").symbol("%").make();

  // DistanceUnit
  /** The base unit of distance. */
  public static final DistanceUnit Meters = BaseUnits.DistanceUnit;

  /** The base unit of distance. */
  public static final DistanceUnit Meter = Meters; // alias

  /** 1/1000 of a {@link #Meter}. */
  public static final DistanceUnit Millimeters = Milli(Meters, "Millimeter", "mm");

  /** 1/1000 of a {@link #Meter}. */
  public static final DistanceUnit Millimeter = Millimeters; // alias

  /** 1/100 of a {@link #Meter}. */
  public static final DistanceUnit Centimeters =
      derive(Meters).splitInto(100).named("Centimeter").symbol("cm").make();

  /** 1/100 of a {@link #Meter}. */
  public static final DistanceUnit Centimeter = Centimeters; // alias

  /** 25.4/1000 of a {@link #Meter} and 1/12 of a {@link #Foot}. */
  public static final DistanceUnit Inches =
      derive(Millimeters).aggregate(25.4).named("Inch").symbol("in").make();

  /** 25.4/1000 of a {@link #Meter} and 1/12 of a {@link #Foot}. */
  public static final DistanceUnit Inch = Inches; // alias

  /** 304.8/1000 of a {@link #Meter}, or 12 {@link #Inches}. */
  public static final DistanceUnit Feet =
      derive(Inches).aggregate(12).named("Foot").symbol("ft").make();

  /** 304.8/1000 of a {@link #Meter}, or 12 {@link #Inches}. */
  public static final DistanceUnit Foot = Feet; // alias

  // TimeUnit
  /** The base unit of time. */
  public static final TimeUnit Seconds = BaseUnits.TimeUnit;

  /** Alias for {@link #Seconds} to make combined unit definitions read more smoothly. */
  public static final TimeUnit Second = Seconds; // singularized alias

  /** 1/1000 of a {@link #Seconds Second}. */
  public static final TimeUnit Milliseconds = Milli(Seconds);

  /** Alias for {@link #Milliseconds} to make combined unit definitions read more smoothly. */
  public static final TimeUnit Millisecond = Milliseconds; // singularized alias

  /** 1/1,000,000 of a {@link #Seconds Second}. */
  public static final TimeUnit Microseconds = Micro(Seconds);

  /** Alias for {@link #Microseconds} to make combined unit definitions read more smoothly. */
  public static final TimeUnit Microsecond = Microseconds; // singularized alias

  /** 60 {@link #Seconds}. */
  public static final TimeUnit Minutes =
      derive(Seconds).aggregate(60).named("Minute").symbol("min").make();

  /** Alias for {@link #Minutes} to make combined unit definitions read more smoothly. */
  public static final TimeUnit Minute = Minutes; // singularized alias

  // AngleUnit
  /**
   * The standard SI unit of angle, represented by the distance that the radius of a unit circle can
   * wrap around its circumference.
   */
  public static final AngleUnit Radians = BaseUnits.AngleUnit;

  /**
   * The standard SI unit of angle, represented by the distance that the radius of a unit circle can
   * wrap around its circumference.
   */
  public static final AngleUnit Radian = Radians; // alias

  /**
   * A single turn of an object around an external axis. Numerically equivalent to {@link
   * #Rotations}, but may be semantically more expressive in certain scenarios.
   */
  public static final AngleUnit Revolutions =
      derive(Radians).aggregate(2 * Math.PI).named("Revolution").symbol("R").make();

  /**
   * A single turn of an object around an external axis. Numerically equivalent to a {@link
   * #Rotation}, but may be semantically more expressive in certain scenarios.
   */
  public static final AngleUnit Revolution = Revolutions; // alias

  /**
   * A single turn of an object around an internal axis. Numerically equivalent to {@link
   * #Revolutions}, but may be semantically more expressive in certain scenarios.
   */
  public static final AngleUnit Rotations =
      derive(Revolutions).named("Rotation").symbol("R").make();

  /**
   * A single turn of an object around an internal axis. Numerically equivalent to a {@link
   * #Revolution}, but may be semantically more expressive in certain scenarios.
   */
  public static final AngleUnit Rotation = Rotations; // alias

  /** 1/360 of a turn around a circle, or 1/57.3 {@link #Radians}. */
  public static final AngleUnit Degrees =
      derive(Revolutions).splitInto(360).named("Degree").symbol("°").make();

  /** 1/360 of a turn around a circle, or 1/57.3 {@link #Radians}. */
  public static final AngleUnit Degree = Degrees; // alias

  // VelocityUnit
  /**
   * The standard SI unit of linear velocity, equivalent to travelling at a rate of one {@link
   * #Meters Meter} per {@link #Second}.
   */
  public static final LinearVelocityUnit MetersPerSecond = Meters.per(Second);

  /**
   * A unit of linear velocity equivalent to travelling at a rate one {@link #Feet Foot} per {@link
   * #Second}.
   */
  public static final LinearVelocityUnit FeetPerSecond = Feet.per(Second);

  /**
   * A unit of linear velocity equivalent to travelling at a rate of one {@link #Inches Inch} per
   * {@link #Second}.
   */
  public static final LinearVelocityUnit InchesPerSecond = Inches.per(Second);

  /**
   * A unit of angular velocity equivalent to spinning at a rate of one {@link #Revolutions
   * Revolution} per {@link #Second}.
   */
  public static final AngularVelocityUnit RevolutionsPerSecond = Revolutions.per(Second);

  /**
   * A unit of angular velocity equivalent to spinning at a rate of one {@link #Rotations Rotation}
   * per {@link #Second}.
   */
  public static final AngularVelocityUnit RotationsPerSecond = Rotations.per(Second);

  /**
   * A unit of angular velocity equivalent to spinning at a rate of one {@link #Rotations Rotation}
   * per {@link #Minute}. Motor spec sheets often list maximum speeds in terms of RPM.
   */
  public static final AngularVelocityUnit RPM = Rotations.per(Minute);

  /**
   * The standard SI unit of angular velocity, equivalent to spinning at a rate of one {@link
   * #Radians Radian} per {@link #Second}.
   */
  public static final AngularVelocityUnit RadiansPerSecond = Radians.per(Second);

  /**
   * A unit of angular velocity equivalent to spinning at a rate of one {@link #Degrees Degree} per
   * {@link #Second}.
   */
  public static final AngularVelocityUnit DegreesPerSecond = Degrees.per(Second);

  /**
   * The standard SI unit of frequency, equivalent to a periodic signal repeating once every {@link
   * #Second}.
   */
  public static final FrequencyUnit Hertz =
      derive(Value.per(Second)).named("Hertz").symbol("hz").make();

  /** 1/1000th of a {@link #Hertz}. */
  public static final FrequencyUnit Millihertz = Milli(Hertz);

  // Acceleration
  /**
   * The standard SI unit of linear acceleration, equivalent to accelerating at a rate of one {@link
   * #Meters Meter} per {@link #Second} every second.
   */
  public static final LinearAccelerationUnit MetersPerSecondPerSecond = MetersPerSecond.per(Second);

  /**
   * A unit of linear acceleration equivalent to accelerating at a rate of one {@link #Foot Foot}
   * per {@link #Second} every second.
   */
  public static final LinearAccelerationUnit FeetPerSecondPerSecond = FeetPerSecond.per(Second);

  /**
   * A unit of angular acceleration equivalent to accelerating at a rate of one {@link #Rotations
   * Rotation} per {@link #Second} every second.
   */
  public static final AngularAccelerationUnit RotationsPerSecondPerSecond =
      RotationsPerSecond.per(Second);

  /**
   * The standard SI unit of angular acceleration, equivalent to accelerating at a rate of one
   * {@link #Radians Radian} per {@link #Second} every second.
   */
  public static final AngularAccelerationUnit RadiansPerSecondPerSecond =
      RadiansPerSecond.per(Second);

  /**
   * A unit of angular acceleration equivalent to accelerating at a rate of one {@link #Degrees
   * Degree} per {@link #Second} every second.
   */
  public static final AngularAccelerationUnit DegreesPerSecondPerSecond =
      DegreesPerSecond.per(Second);

  /**
   * A unit of acceleration equivalent to the pull of gravity on an object at sea level on Earth.
   */
  public static final LinearAccelerationUnit Gs =
      derive(MetersPerSecondPerSecond).aggregate(9.80665).named("G").symbol("G").make();

  // MassUnit
  /** The standard SI unit of mass. */
  public static final MassUnit Kilograms = BaseUnits.MassUnit;

  /** The standard SI unit of mass. */
  public static final MassUnit Kilogram = Kilograms; // alias

  /** 1/1000 of a {@link #Kilogram}. */
  public static final MassUnit Grams = Milli(Kilograms, "Gram", "g");

  /** 1/1000 of a {@link #Kilogram}. */
  public static final MassUnit Gram = Grams; // alias

  /**
   * A unit of mass equivalent to approximately 453 {@link #Grams}. This is <i>not</i> equivalent to
   * pounds-force, which is the amount of force required to accelerate an object with one pound of
   * mass at a rate of one {@link #Gs G}.
   *
   * @see #PoundsForce
   */
  public static final MassUnit Pounds =
      derive(Grams).aggregate(453.592).named("Pound").symbol("lb.").make();

  /**
   * A unit of mass equivalent to approximately 453 {@link #Grams}. This is <i>not</i> equivalent to
   * pounds-force, which is the amount of force required to accelerate an object with one pound of
   * mass at a rate of one {@link #Gs G}.
   *
   * @see #PoundForce
   */
  public static final MassUnit Pound = Pounds; // alias

  /** 1/16 of a {@link #Pound}. */
  public static final MassUnit Ounces =
      derive(Pounds).splitInto(16).named("Ounce").symbol("oz.").make();

  /** 1/16 of a {@link #Pound}. */
  public static final MassUnit Ounce = Ounces; // alias

  // Force

  /**
   * The standard unit of force, equivalent to accelerating a mass of one {@link #Kilogram} at a
   * rate of one {@link #MetersPerSecondPerSecond meter per second per second}.
   */
  public static final ForceUnit Newtons =
      derive(Kilograms.mult(MetersPerSecondPerSecond)).named("Newton").symbol("N").make();

  /**
   * The standard unit of force, equivalent to the standard force of gravity applied to a one {@link
   * #Kilogram} mass.
   */
  public static final ForceUnit Newton = Newtons;

  /**
   * The standard Imperial unit of force, equivalent to the standard force of gravity applied to a
   * one {@link #Pound} mass.
   */
  public static final ForceUnit PoundsForce =
      derive(Pounds.mult(Gs)).named("Pound-force").symbol("lbsf.").make();

  /**
   * The standard Imperial unit of force, equivalent to the standard force of gravity applied to a
   * one {@link #Pound} mass.
   */
  public static final ForceUnit PoundForce = PoundsForce;

  /**
   * 1/16th of {@link #PoundsForce}, equivalent to the standard force of gravity applied to a one
   * {@link #Ounce} mass.
   */
  public static final ForceUnit OuncesForce =
      derive(Ounces.mult(Gs)).named("Ounce-force").symbol("ozf").make();

  /**
   * 1/16th of {@link #PoundsForce}, equivalent to the standard force of gravity applied to a one
   * {@link #Ounce} mass.
   */
  public static final ForceUnit OunceForce = OuncesForce;

  // Torque

  /** The standard SI unit for torque. */
  public static final TorqueUnit NewtonMeters = Meters.multAsTorque(Newtons);

  /** The standard SI unit for torque. */
  public static final TorqueUnit NewtonMeter = NewtonMeters;

  /**
   * The equivalent of one {@link #PoundsForce pound of force} applied to an object one {@link
   * #Foot} away from its center of rotation.
   */
  public static final TorqueUnit PoundFeet = Feet.multAsTorque(PoundsForce);

  /**
   * The equivalent of one {@link #PoundsForce pound of force} applied to an object one {@link
   * #Foot} away from its center of rotation.
   */
  public static final TorqueUnit PoundFoot = PoundFeet;

  /**
   * The equivalent of one {@link #PoundsForce pound of force} applied to an object one {@link
   * #Inch} away from its center of rotation.
   */
  public static final TorqueUnit PoundInches = Inches.multAsTorque(PoundsForce);

  /**
   * The equivalent of one {@link #PoundsForce pound of force} applied to an object one {@link
   * #Inch} away from its center of rotation.
   */
  public static final TorqueUnit PoundInch = PoundInches;

  /**
   * The equivalent of one {@link #OunceForce ounce of force} applied to an object one {@link #Inch}
   * away from its center of rotation.
   */
  public static final TorqueUnit OunceInches = Inches.multAsTorque(OuncesForce);

  /**
   * The equivalent of one {@link #OunceForce ounce of force} applied to an object one {@link #Inch}
   * away from its center of rotation.
   */
  public static final TorqueUnit OunceInch = OunceInches;

  // Linear momentum

  /**
   * The standard SI unit for linear momentum, equivalent to a one {@link #Kilogram} mass moving at
   * one {@link #MetersPerSecond}.
   */
  public static final LinearMomentumUnit KilogramMetersPerSecond = Kilograms.mult(MetersPerSecond);

  // Angular momentum

  /** The standard SI unit for angular momentum. */
  public static final AngularMomentumUnit KilogramMetersSquaredPerSecond =
      KilogramMetersPerSecond.mult(Meters);

  // Moment of Inertia

  /** The standard SI unit for moment of inertia. */
  public static final MomentOfInertiaUnit KilogramSquareMeters =
      KilogramMetersSquaredPerSecond.mult(RadiansPerSecond);

  // VoltageUnit
  /** The base unit of electric potential. */
  public static final VoltageUnit Volts = BaseUnits.VoltageUnit;

  /** The base unit of electric potential. */
  public static final VoltageUnit Volt = Volts; // alias

  /**
   * 1/1000 of a {@link #Volt}. Useful when dealing with low-voltage applications like LED drivers
   * or low-power circuits.
   */
  public static final VoltageUnit Millivolts = Milli(Volts);

  /**
   * 1/1000 of a {@link #Volt}. Useful when dealing with low-voltage applications like LED drivers
   * or low-power circuits.
   */
  public static final VoltageUnit Millivolt = Millivolts; // alias

  // CurrentUnit
  /** The base unit of electrical current. */
  public static final CurrentUnit Amps = BaseUnits.CurrentUnit;

  /** The base unit of electrical current. */
  public static final CurrentUnit Amp = Amps; // alias

  /**
   * A unit equal to 1/1000 of an {@link #Amp}. Useful when dealing with low-current applications
   * like LED drivers or low-power circuits.
   */
  public static final CurrentUnit Milliamps = Milli(Amps);

  /**
   * A unit equal to 1/1000 of an {@link #Amp}. Useful when dealing with low-current applications
   * like LED drivers or low-power circuits.
   */
  public static final CurrentUnit Milliamp = Milliamps; // alias

  // ResistanceUnit
  /** The base unit of resistance. Equivalent to one {@link #Volt} per {@link #Amp}. */
  public static final ResistanceUnit Ohms = derive(Volts.per(Amp)).named("Ohm").symbol("Ω").make();

  /** The base unit of resistance. Equivalent to one {@link #Volt} per {@link #Amp}. */
  public static final ResistanceUnit Ohm = Ohms; // alias

  /** A unit equal to 1,000 {@link #Ohms}. */
  public static final ResistanceUnit KiloOhms = Kilo(Ohms);

  /** A unit equal to 1,000 {@link #Ohms}. */
  public static final ResistanceUnit KiloOhm = KiloOhms; // alias

  /** A unit equal to 1/1000 of a {@link #Ohm}. */
  public static final ResistanceUnit MilliOhms = Milli(Ohms);

  /** A unit equal to 1/1000 of a {@link #Ohm}. */
  public static final ResistanceUnit MilliOhm = MilliOhms; // alias

  // EnergyUnit
  /** The base unit of energy. */
  public static final EnergyUnit Joules = BaseUnits.EnergyUnit;

  /** The base unit of energy. */
  public static final EnergyUnit Joule = Joules; // alias

  /**
   * A unit equal to 1/1000 of a {@link #Joule}. Useful when dealing with lower-power applications.
   */
  public static final EnergyUnit Millijoules = Milli(Joules);

  /**
   * A unit equal to 1/1000 of a {@link #Joule}. Useful when dealing with lower-power applications.
   */
  public static final EnergyUnit Millijoule = Millijoules; // alias

  /**
   * A unit equal to 1,000 {@link #Joules}. Useful when dealing with higher-level robot energy
   * usage.
   */
  public static final EnergyUnit Kilojoules = Kilo(Joules);

  /**
   * A unit equal to 1,000 {@link #Joules}. Useful when dealing with higher-level robot energy
   * usage.
   */
  public static final EnergyUnit Kilojoule = Kilojoules; // alias

  // PowerUnit
  /** The base unit of power. Equivalent to one {@link #Joule} per {@link #Second}. */
  public static final PowerUnit Watts = derive(Joules.per(Second)).named("Watt").symbol("W").make();

  /** The base unit of power. Equivalent to one {@link #Joule} per {@link #Second}. */
  public static final PowerUnit Watt = Watts; // alias

  /**
   * A unit equal to 1/1000 of a {@link #Watt}. Useful when dealing with lower-power applications.
   */
  public static final PowerUnit Milliwatts = Milli(Watts);

  /**
   * A unit equal to 1/1000 of a {@link #Watt}. Useful when dealing with lower-power applications.
   */
  public static final PowerUnit Milliwatt = Milliwatts; // alias

  /**
   * A unit equal to 745.7 {@link #Watts}. May be useful when dealing with high-power gearboxes and
   * motors.
   */
  public static final PowerUnit Horsepower =
      derive(Watts).aggregate(745.7).named("Horsepower").symbol("HP").make();

  // TemperatureUnit
  /**
   * The base unit of temperature, where a value of 0 corresponds with absolutely zero energy in the
   * measured system. Not particularly useful for robots unless you're cooling motors with liquid
   * helium.
   */
  public static final TemperatureUnit Kelvin = BaseUnits.TemperatureUnit;

  /**
   * The standard SI unit of temperature, where a value of 0 roughly corresponds to the freezing
   * point of water and a value of 100 corresponds to the boiling point. Electronics tend to exhibit
   * degraded performance or damage above 90 degrees Celsius.
   */
  public static final TemperatureUnit Celsius =
      derive(Kelvin).offset(+273.15).named("Celsius").symbol("°C").make();

  /**
   * The base imperial (American) unit of temperature, where a value of 32 roughly corresponds to
   * the freezing point of water and a value of 212 corresponds to the boiling point.
   */
  public static final TemperatureUnit Fahrenheit =
      derive(Celsius)
          .mappingInputRange(0, 100)
          .toOutputRange(32, 212)
          .named("Fahrenheit")
          .symbol("°F")
          .make();

  // Standard feedforward units for kV and kA.
  // kS and kG are just volts, which is already defined earlier
  /**
   * A standard unit for measuring linear mechanisms' feedforward voltages based on a model of the
   * system and a desired commanded linear velocity.
   */
  public static final PerUnit<VoltageUnit, LinearVelocityUnit> VoltsPerMeterPerSecond =
      Volts.per(MetersPerSecond);

  /**
   * A standard unit for measuring linear mechanisms' feedforward voltages based on a model of the
   * system and a desired commanded linear acceleration.
   */
  public static final PerUnit<VoltageUnit, LinearAccelerationUnit> VoltsPerMeterPerSecondSquared =
      Volts.per(MetersPerSecondPerSecond);

  /**
   * A standard unit for measuring angular mechanisms' feedforward voltages based on a model of the
   * system and a desired commanded angular velocity.
   */
  public static final PerUnit<VoltageUnit, AngularVelocityUnit> VoltsPerRadianPerSecond =
      Volts.per(RadiansPerSecond);

  /**
   * A standard unit for measuring angular mechanisms' feedforward voltages based on a model of the
   * system and a desired commanded angular acceleration.
   */
  public static final PerUnit<VoltageUnit, AngularAccelerationUnit> VoltsPerRadianPerSecondSquared =
      Volts.per(RadiansPerSecond.per(Second));

  /**
   * Creates a unit equal to a thousandth of the base unit, eg Milliseconds = Milli(Units.Seconds).
   *
   * @param <U> the type of the unit
   * @param baseUnit the unit being derived from. This does not have to be the base unit of measure
   * @param name the name of the new derived unit
   * @param symbol the symbol of the new derived unit
   * @return the milli-unit
   */
  @SuppressWarnings("checkstyle:methodname")
  public static <U extends Unit> U Milli(U baseUnit, String name, String symbol) {
    return derive(baseUnit).splitInto(1000).named(name).symbol(symbol).make();
  }

  /**
   * Creates a unit equal to a thousandth of the base unit, eg Milliseconds = Milli(Units.Seconds).
   *
   * @param <U> the type of the unit
   * @param baseUnit the unit being derived from. This does not have to be the base unit of measure
   * @return the milli-unit
   */
  @SuppressWarnings("checkstyle:methodname")
  public static <U extends Unit> U Milli(U baseUnit) {
    return Milli(
        baseUnit, "Milli" + baseUnit.name().toLowerCase(Locale.ROOT), "m" + baseUnit.symbol());
  }

  /**
   * Creates a unit equal to a millionth of the base unit, eg {@code Microseconds =
   * Micro(Units.Seconds, "Microseconds", 'us")}.
   *
   * @param <U> the type of the unit
   * @param baseUnit the unit being derived from. This does not have to be the base unit of measure
   * @param name the name of the new derived unit
   * @param symbol the symbol of the new derived unit
   * @return the micro-unit
   */
  @SuppressWarnings("checkstyle:methodname")
  public static <U extends Unit> U Micro(U baseUnit, String name, String symbol) {
    return derive(baseUnit).splitInto(1_000_000).named(name).symbol(symbol).make();
  }

  /**
   * Creates a unit equal to a millionth of the base unit, eg Microseconds = Micro(Units.Seconds).
   *
   * @param <U> the type of the unit
   * @param baseUnit the unit being derived from. This does not have to be the base unit of measure
   * @return the micro-unit
   */
  @SuppressWarnings("checkstyle:methodname")
  public static <U extends Unit> U Micro(U baseUnit) {
    return Micro(
        baseUnit, "Micro" + baseUnit.name().toLowerCase(Locale.ROOT), "u" + baseUnit.symbol());
  }

  /**
   * Creates a unit equal to a thousand of the base unit, eg Kilograms = Kilo(Units.Grams).
   *
   * @param <U> the type of the unit
   * @param baseUnit the unit being derived from. This does not have to be the base unit of measure
   * @param name the name of the new derived unit
   * @param symbol the symbol of the new derived unit
   * @return the kilo-unit
   */
  @SuppressWarnings("checkstyle:methodname")
  public static <U extends Unit> U Kilo(U baseUnit, String name, String symbol) {
    return derive(baseUnit).aggregate(1000).named(name).symbol(symbol).make();
  }

  /**
   * Creates a unit equal to a thousand of the base unit, eg Kilograms = Kilo(Units.Grams).
   *
   * @param <U> the type of the unit
   * @param baseUnit the unit being derived from. This does not have to be the base unit of measure
   * @return the kilo-unit
   */
  @SuppressWarnings("checkstyle:methodname")
  public static <U extends Unit> U Kilo(U baseUnit) {
    return Kilo(
        baseUnit, "Kilo" + baseUnit.name().toLowerCase(Locale.ROOT), "K" + baseUnit.symbol());
  }

  /**
   * Creates a new unit builder object based on a given input unit. The builder can be used to
   * fluently describe a new unit in terms of its relation to the base unit.
   *
   * @param unit the base unit from which to derive a new unit
   * @param <U> the dimension of the unit to derive
   * @return a builder object
   */
  public static <U extends Unit> UnitBuilder<U> derive(U unit) {
    return new UnitBuilder<>(unit);
  }
}
