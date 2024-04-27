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

  /**
   * Used as an internal placeholder value when a specific unit type cannot be determined. Do not
   * use this directly.
   */
  @SuppressWarnings("rawtypes")
  public static final Unit AnonymousBaseUnit = new Dimensionless(1, "<?>", "<?>");

  // Distance
  /** The base unit of distance. */
  public static final Distance Meters = BaseUnits.Distance;

  /** The base unit of distance. */
  public static final Distance Meter = Meters; // alias

  /** 1/1000 of a {@link #Meter}. */
  public static final Distance Millimeters = Milli(Meters, "Millimeter", "mm");

  /** 1/1000 of a {@link #Meter}. */
  public static final Distance Millimeter = Millimeters; // alias

  /** 1/100 of a {@link #Meter}. */
  public static final Distance Centimeters =
      derive(Meters).splitInto(100).named("Centimeter").symbol("cm").make();

  /** 1/100 of a {@link #Meter}. */
  public static final Distance Centimeter = Centimeters; // alias

  /** 25.4/1000 of a {@link #Meter} and 1/12 of a {@link #Foot}. */
  public static final Distance Inches =
      derive(Millimeters).aggregate(25.4).named("Inch").symbol("in").make();

  /** 25.4/1000 of a {@link #Meter} and 1/12 of a {@link #Foot}. */
  public static final Distance Inch = Inches; // alias

  /** 304.8/1000 of a {@link #Meter}, or 12 {@link #Inches}. */
  public static final Distance Feet =
      derive(Inches).aggregate(12).named("Foot").symbol("ft").make();

  /** 304.8/1000 of a {@link #Meter}, or 12 {@link #Inches}. */
  public static final Distance Foot = Feet; // alias

  // Time
  /** The base unit of time. */
  public static final Time Seconds = BaseUnits.Time;

  /** Alias for {@link #Seconds} to make combined unit definitions read more smoothly. */
  public static final Time Second = Seconds; // singularized alias

  /** 1/1000 of a {@link #Seconds Second}. */
  public static final Time Milliseconds = Milli(Seconds);

  /** Alias for {@link #Milliseconds} to make combined unit definitions read more smoothly. */
  public static final Time Millisecond = Milliseconds; // singularized alias

  /** 1/1,000,000 of a {@link #Seconds Second}. */
  public static final Time Microseconds = Micro(Seconds);

  /** Alias for {@link #Microseconds} to make combined unit definitions read more smoothly. */
  public static final Time Microsecond = Microseconds; // singularized alias

  /** 60 {@link #Seconds}. */
  public static final Time Minutes =
      derive(Seconds).aggregate(60).named("Minute").symbol("min").make();

  /** Alias for {@link #Minutes} to make combined unit definitions read more smoothly. */
  public static final Time Minute = Minutes; // singularized alias

  // Angle
  /**
   * The base SI unit of angle, represented by the distance that the radius of a unit circle can
   * wrap around its circumference.
   */
  public static final Angle Radians = BaseUnits.Angle;

  /**
   * The base SI unit of angle, represented by the distance that the radius of a unit circle can
   * wrap around its circumference.
   */
  public static final Angle Radian = Radians; // alias

  /**
   * A single turn of an object around an external axis. Numerically equivalent to {@link
   * #Rotations}, but may be semantically more expressive in certain scenarios.
   */
  public static final Angle Revolutions = new Angle(2 * Math.PI, "Revolution", "R");

  /**
   * A single turn of an object around an external axis. Numerically equivalent to a {@link
   * #Rotation}, but may be semantically more expressive in certain scenarios.
   */
  public static final Angle Revolution = Revolutions; // alias

  /**
   * A single turn of an object around an internal axis. Numerically equivalent to {@link
   * #Revolutions}, but may be semantically more expressive in certain scenarios.
   */
  public static final Angle Rotations = new Angle(2 * Math.PI, "Rotation", "R"); // alias revolution

  /**
   * A single turn of an object around an internal axis. Numerically equivalent to a {@link
   * #Revolution}, but may be semantically more expressive in certain scenarios.
   */
  public static final Angle Rotation = Rotations; // alias

  /** 1/360 of a turn around a circle, or 1/57.3 {@link #Radians}. */
  public static final Angle Degrees =
      derive(Revolutions).splitInto(360).named("Degree").symbol("°").make();

  /** 1/360 of a turn around a circle, or 1/57.3 {@link #Radians}. */
  public static final Angle Degree = Degrees; // alias

  // Velocity
  /**
   * The standard SI unit of linear velocity, equivalent to travelling at a rate of one {@link
   * #Meters Meter} per {@link #Second}.
   */
  public static final Velocity<Distance> MetersPerSecond = Meters.per(Second);

  /**
   * A unit of linear velocity equivalent to travelling at a rate one {@link #Feet Foot} per {@link
   * #Second}.
   */
  public static final Velocity<Distance> FeetPerSecond = Feet.per(Second);

  /**
   * A unit of linear velocity equivalent to travelling at a rate of one {@link #Inches Inch} per
   * {@link #Second}.
   */
  public static final Velocity<Distance> InchesPerSecond = Inches.per(Second);

  /**
   * A unit of angular velocity equivalent to spinning at a rate of one {@link #Revolutions
   * Revolution} per {@link #Second}.
   */
  public static final Velocity<Angle> RevolutionsPerSecond = Revolutions.per(Second);

  /**
   * A unit of angular velocity equivalent to spinning at a rate of one {@link #Rotations Rotation}
   * per {@link #Second}.
   */
  public static final Velocity<Angle> RotationsPerSecond = Rotations.per(Second);

  /**
   * A unit of angular velocity equivalent to spinning at a rate of one {@link #Rotations Rotation}
   * per {@link #Minute}. Motor spec sheets often list maximum speeds in terms of RPM.
   */
  public static final Velocity<Angle> RPM = Rotations.per(Minute);

  /**
   * The standard SI unit of angular velocity, equivalent to spinning at a rate of one {@link
   * #Radians Radian} per {@link #Second}.
   */
  public static final Velocity<Angle> RadiansPerSecond = Radians.per(Second);

  /**
   * A unit of angular velocity equivalent to spinning at a rate of one {@link #Degrees Degree} per
   * {@link #Second}.
   */
  public static final Velocity<Angle> DegreesPerSecond = Degrees.per(Second);

  // Acceleration
  /**
   * The standard SI unit of linear acceleration, equivalent to accelerating at a rate of one {@link
   * #Meters Meter} per {@link #Second} every second.
   */
  public static final Velocity<Velocity<Distance>> MetersPerSecondPerSecond =
      MetersPerSecond.per(Second);

  /**
   * A unit of angular acceleration equivalent to accelerating at a rate of one {@link #Rotations
   * Rotation} per {@link #Second} every second.
   */
  public static final Velocity<Velocity<Angle>> RotationsPerSecondPerSecond =
      RotationsPerSecond.per(Second);

  /**
   * The standard SI unit of angular acceleration, equivalent to accelerating at a rate of one
   * {@link #Radians Radian} per {@link #Second} every second.
   */
  public static final Velocity<Velocity<Angle>> RadiansPerSecondPerSecond =
      RadiansPerSecond.per(Second);

  /**
   * A unit of angular acceleration equivalent to accelerating at a rate of one {@link #Degrees
   * Degree} per {@link #Second} every second.
   */
  public static final Velocity<Velocity<Angle>> DegreesPerSecondPerSecond =
      DegreesPerSecond.per(Second);

  /**
   * A unit of acceleration equivalent to the pull of gravity on an object at sea level on Earth.
   */
  public static final Velocity<Velocity<Distance>> Gs =
      derive(MetersPerSecondPerSecond).aggregate(9.80665).named("G").symbol("G").make();

  // Mass
  /** The base SI unit of mass. */
  public static final Mass Kilograms = BaseUnits.Mass;

  /** The base SI unit of mass. */
  public static final Mass Kilogram = Kilograms; // alias

  /** 1/1000 of a {@link #Kilogram}. */
  public static final Mass Grams = Milli(Kilograms, "Gram", "g");

  /** 1/1000 of a {@link #Kilogram}. */
  public static final Mass Gram = Grams; // alias

  /**
   * A unit of mass equivalent to approximately 453 {@link #Grams}. This is <i>not</i> equivalent to
   * pounds-force, which is the amount of force required to accelerate an object with one pound of
   * mass at a rate of one {@link #Gs G}.
   */
  public static final Mass Pounds =
      derive(Grams).aggregate(453.592).named("Pound").symbol("lb.").make();

  /**
   * A unit of mass equivalent to approximately 453 {@link #Grams}. This is <i>not</i> equivalent to
   * pounds-force, which is the amount of force required to accelerate an object with one pound of
   * mass at a rate of one {@link #Gs G}.
   */
  public static final Mass Pound = Pounds; // alias

  /** 1/16 of a {@link #Pound}. */
  public static final Mass Ounces =
      derive(Pounds).splitInto(16).named("Ounce").symbol("oz.").make();

  /** 1/16 of a {@link #Pound}. */
  public static final Mass Ounce = Ounces; // alias

  // Moment of Inertia
  /** The base SI unit for moment of inertia. */
  public static final Mult<Mult<Mass, Distance>, Distance> KilogramSquareMeters =
      Kilograms.mult(Meters).mult(Meters);

  // Unitless
  /** A dimensionless unit that performs no scaling whatsoever. */
  public static final Dimensionless Value = BaseUnits.Value;

  /**
   * A dimensionless unit equal to to 1/100th of a {@link #Value}. A measurement of {@code
   * Percent.of(42)} would be equivalent to {@code Value.of(0.42)}.
   */
  public static final Dimensionless Percent =
      derive(Value).splitInto(100).named("Percent").symbol("%").make();

  // Voltage
  /** The base unit of electric potential. */
  public static final Voltage Volts = BaseUnits.Voltage;

  /** The base unit of electric potential. */
  public static final Voltage Volt = Volts; // alias

  /**
   * 1/1000 of a {@link #Volt}. Useful when dealing with low-voltage applications like LED drivers
   * or low-power circuits.
   */
  public static final Voltage Millivolts = Milli(Volts);

  /**
   * 1/1000 of a {@link #Volt}. Useful when dealing with low-voltage applications like LED drivers
   * or low-power circuits.
   */
  public static final Voltage Millivolt = Millivolts; // alias

  // Current
  /** The base unit of electrical current. */
  public static final Current Amps = BaseUnits.Current;

  /** The base unit of electrical current. */
  public static final Current Amp = Amps; // alias

  /**
   * A unit equal to 1/1000 of an {@link #Amp}. Useful when dealing with low-current applications
   * like LED drivers or low-power circuits.
   */
  public static final Current Milliamps = Milli(Amps);

  /**
   * A unit equal to 1/1000 of an {@link #Amp}. Useful when dealing with low-current applications
   * like LED drivers or low-power circuits.
   */
  public static final Current Milliamp = Milliamps; // alias

  // Energy
  /** The base unit of energy. */
  public static final Energy Joules = BaseUnits.Energy;

  /** The base unit of energy. */
  public static final Energy Joule = Joules; // alias

  /**
   * A unit equal to 1/1000 of a {@link #Joule}. Useful when dealing with lower-power applications.
   */
  public static final Energy Millijoules = Milli(Joules);

  /**
   * A unit equal to 1/1000 of a {@link #Joule}. Useful when dealing with lower-power applications.
   */
  public static final Energy Millijoule = Millijoules; // alias

  /**
   * A unit equal to 1,000 {@link #Joules}. Useful when dealing with higher-level robot energy
   * usage.
   */
  public static final Energy Kilojoules = Kilo(Joules);

  /**
   * A unit equal to 1,000 {@link #Joules}. Useful when dealing with higher-level robot energy
   * usage.
   */
  public static final Energy Kilojoule = Kilojoules; // alias

  // Power
  /** The base unit of power. Equivalent to one {@link #Joule} per {@link #Second}. */
  public static final Power Watts = BaseUnits.Power;

  /** The base unit of power. Equivalent to one {@link #Joule} per {@link #Second}. */
  public static final Power Watt = Watts; // alias

  /**
   * A unit equal to 1/1000 of a {@link #Watt}. Useful when dealing with lower-power applications.
   */
  public static final Power Milliwatts = Milli(Watts);

  /**
   * A unit equal to 1/1000 of a {@link #Watt}. Useful when dealing with lower-power applications.
   */
  public static final Power Milliwatt = Milliwatts; // alias

  /**
   * A unit equal to 745.7 {@link #Watts}. May be useful when dealing with high-power gearboxes and
   * motors.
   */
  public static final Power Horsepower =
      derive(Watts).aggregate(745.7).named("Horsepower").symbol("HP").make();

  // Temperature
  /**
   * The base unit of temperature, where a value of 0 corresponds with absolutely zero energy in the
   * measured system. Not particularly useful for robots unless you're cooling motors with liquid
   * helium.
   */
  public static final Temperature Kelvin = BaseUnits.Temperature;

  /**
   * The base SI unit of temperature, where a value of 0 roughly corresponds to the freezing point
   * of water and a value of 100 corresponds to the boiling point. Electronics tend to exhibit
   * degraded performance or damage above 90 degrees Celsius.
   */
  public static final Temperature Celsius =
      derive(Kelvin).offset(+273.15).named("Celsius").symbol("°C").make();

  /**
   * The base imperial (American) unit of temperature, where a value of 32 roughly corresponds to
   * the freezing point of water and a value of 212 corresponds to the boiling point.
   */
  public static final Temperature Fahrenheit =
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
  public static final Per<Voltage, Velocity<Distance>> VoltsPerMeterPerSecond =
      Volts.per(MetersPerSecond);

  /**
   * A standard unit for measuring linear mechanisms' feedforward voltages based on a model of the
   * system and a desired commanded linear acceleration.
   */
  public static final Per<Voltage, Velocity<Velocity<Distance>>> VoltsPerMeterPerSecondSquared =
      Volts.per(MetersPerSecondPerSecond);

  /**
   * A standard unit for measuring angular mechanisms' feedforward voltages based on a model of the
   * system and a desired commanded angular velocity.
   */
  public static final Per<Voltage, Velocity<Angle>> VoltsPerRadianPerSecond =
      Volts.per(RadiansPerSecond);

  /**
   * A standard unit for measuring angular mechanisms' feedforward voltages based on a model of the
   * system and a desired commanded angular acceleration.
   */
  public static final Per<Voltage, Velocity<Velocity<Angle>>> VoltsPerRadianPerSecondSquared =
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
  @SuppressWarnings({"PMD.MethodName", "checkstyle:methodname"})
  public static <U extends Unit<U>> U Milli(Unit<U> baseUnit, String name, String symbol) {
    return derive(baseUnit).splitInto(1000).named(name).symbol(symbol).make();
  }

  /**
   * Creates a unit equal to a thousandth of the base unit, eg Milliseconds = Milli(Units.Seconds).
   *
   * @param <U> the type of the unit
   * @param baseUnit the unit being derived from. This does not have to be the base unit of measure
   * @return the milli-unit
   */
  @SuppressWarnings({"PMD.MethodName", "checkstyle:methodname"})
  public static <U extends Unit<U>> U Milli(Unit<U> baseUnit) {
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
  @SuppressWarnings({"PMD.MethodName", "checkstyle:methodname"})
  public static <U extends Unit<U>> U Micro(Unit<U> baseUnit, String name, String symbol) {
    return derive(baseUnit).splitInto(1_000_000).named(name).symbol(symbol).make();
  }

  /**
   * Creates a unit equal to a millionth of the base unit, eg Microseconds = Micro(Units.Seconds).
   *
   * @param <U> the type of the unit
   * @param baseUnit the unit being derived from. This does not have to be the base unit of measure
   * @return the micro-unit
   */
  @SuppressWarnings({"PMD.MethodName", "checkstyle:methodname"})
  public static <U extends Unit<U>> U Micro(Unit<U> baseUnit) {
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
  @SuppressWarnings({"PMD.MethodName", "checkstyle:methodname"})
  public static <U extends Unit<U>> U Kilo(Unit<U> baseUnit, String name, String symbol) {
    return derive(baseUnit).aggregate(1000).named(name).symbol(symbol).make();
  }

  /**
   * Creates a unit equal to a thousand of the base unit, eg Kilograms = Kilo(Units.Grams).
   *
   * @param <U> the type of the unit
   * @param baseUnit the unit being derived from. This does not have to be the base unit of measure
   * @return the kilo-unit
   */
  @SuppressWarnings({"PMD.MethodName", "checkstyle:methodname"})
  public static <U extends Unit<U>> U Kilo(Unit<U> baseUnit) {
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
  @SuppressWarnings("unchecked")
  public static <U extends Unit<U>> UnitBuilder<U> derive(Unit<U> unit) {
    return new UnitBuilder<>((U) unit);
  }

  /**
   * Returns an anonymous unit for use when a specific unit type is not known. Do not use this
   * directly.
   *
   * @param <U> the dimension of the desired anonymous unit
   * @return the anonymous unit
   */
  @SuppressWarnings("unchecked")
  public static <U extends Unit<U>> U anonymous() {
    return (U) AnonymousBaseUnit;
  }
}
