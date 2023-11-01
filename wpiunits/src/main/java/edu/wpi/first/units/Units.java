// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import java.util.Locale;

public final class Units {
  private Units() {
    // Prevent instantiation
  }

  // Pseudo-classes describing the more common units of measure.

  @SuppressWarnings("rawtypes")
  public static final Unit AnonymousBaseUnit = new Dimensionless(1, "<?>", "<?>");

  // Distance
  public static final Distance Meters = BaseUnits.Distance;
  public static final Distance Millimeters = Milli(Meters, "Millimeter", "mm");
  public static final Distance Centimeters =
      derive(Meters).splitInto(100).named("Centimeter").symbol("cm").make();
  public static final Distance Inches =
      derive(Millimeters).aggregate(25.4).named("Inch").symbol("in").make();
  public static final Distance Feet =
      derive(Inches).aggregate(12).named("Foot").symbol("ft").make();

  // Time
  public static final Time Seconds = BaseUnits.Time;
  public static final Time Second = Seconds; // singularized alias
  public static final Time Milliseconds = Milli(Seconds);
  public static final Time Millisecond = Milliseconds; // singularized alias
  public static final Time Microseconds = Micro(Seconds);
  public static final Time Microsecond = Microseconds; // singularized alias
  public static final Time Minutes =
      derive(Seconds).aggregate(60).named("Minute").symbol("min").make();
  public static final Time Minute = Minutes; // singularized alias

  // Angle
  public static final Angle Revolutions = BaseUnits.Angle;
  public static final Angle Rotations = new Angle(1, "Rotation", "R"); // alias
  public static final Angle Radians =
      derive(Revolutions).splitInto(2 * Math.PI).named("Radian").symbol("rad").make();
  public static final Angle Degrees =
      derive(Revolutions).splitInto(360).named("Degree").symbol("°").make();

  // Velocity
  public static final Velocity<Distance> MetersPerSecond = Meters.per(Second);
  public static final Velocity<Distance> FeetPerSecond = Feet.per(Second);
  public static final Velocity<Distance> InchesPerSecond = Inches.per(Second);

  public static final Velocity<Angle> RevolutionsPerSecond = Revolutions.per(Second);
  public static final Velocity<Angle> RotationsPerSecond = Rotations.per(Second);
  public static final Velocity<Angle> RPM = Rotations.per(Minute);
  public static final Velocity<Angle> RadiansPerSecond = Radians.per(Second);
  public static final Velocity<Angle> DegreesPerSecond = Degrees.per(Second);

  // Acceleration
  public static final Velocity<Velocity<Distance>> MetersPerSecondPerSecond =
      MetersPerSecond.per(Second);
  public static final Velocity<Velocity<Distance>> Gs =
      derive(MetersPerSecondPerSecond).aggregate(9.80665).named("G").symbol("G").make();

  // Mass
  public static final Mass Kilograms = BaseUnits.Mass;
  public static final Mass Grams = Milli(Kilograms, "Gram", "g");
  public static final Mass Pounds =
      derive(Grams).aggregate(453.592).named("Pound").symbol("lb.").make();
  public static final Mass Ounces =
      derive(Pounds).splitInto(16).named("Ounce").symbol("oz.").make();

  // Unitless
  public static final Dimensionless Value = BaseUnits.Value;
  public static final Dimensionless Percent =
      derive(Value).splitInto(100).named("Percent").symbol("%").make();

  // Voltage
  public static final Voltage Volts = BaseUnits.Voltage;
  public static final Voltage Millivolts = Milli(Volts);

  // Current
  public static final Current Amps = BaseUnits.Current;
  public static final Current Milliamps = Milli(Amps);

  // Energy
  public static final Energy Joules = BaseUnits.Energy;
  public static final Energy Millijoules = Milli(Joules);
  public static final Energy Kilojoules = Kilo(Joules);

  // Power
  public static final Power Watts = BaseUnits.Power;
  public static final Power Milliwatts = Milli(Watts);
  public static final Power Horsepower =
      derive(Watts).aggregate(745.7).named("Horsepower").symbol("HP").make();

  // Temperature
  public static final Temperature Kelvin = BaseUnits.Temperature;
  public static final Temperature Celsius =
      derive(Kelvin).offset(+273.15).named("Celsius").symbol("°C").make();

  public static final Temperature Fahrenheit =
      derive(Celsius)
          .mappingInputRange(0, 100)
          .toOutputRange(32, 212)
          .named("Fahrenheit")
          .symbol("°F")
          .make();

  // Standard feedforward units for kV and kA.
  // kS and kG are just volts, which is already defined earlier
  public static final Per<Voltage, Velocity<Distance>> VoltsPerMeterPerSecond =
      Volts.per(MetersPerSecond);
  public static final Per<Voltage, Velocity<Velocity<Distance>>> VoltsPerMeterPerSecondSquared =
      Volts.per(MetersPerSecondPerSecond);

  public static final Per<Voltage, Velocity<Angle>> VoltsPerRadianPerSecond =
      Volts.per(RadiansPerSecond);
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

  @SuppressWarnings("unchecked")
  public static <U extends Unit<U>> UnitBuilder<U> derive(Unit<U> unit) {
    return new UnitBuilder<>((U) unit);
  }

  @SuppressWarnings("unchecked")
  public static <U extends Unit<U>> U anonymous() {
    return (U) AnonymousBaseUnit;
  }
}
