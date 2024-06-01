// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

/** The base units of measure. */
public final class BaseUnits {
  private BaseUnits() {
    // Prevent instantiation
  }

  /** The standard unit of distance, meters. */
  public static final Distance Distance = new Distance(null, 1, "Meter", "m");

  /** The standard unit of time, seconds. */
  public static final Time Time = new Time(null, 1, "Second", "s");

  /** The standard unit of mass, kilograms. */
  public static final Mass Mass = new Mass(null, 1, "Kilogram", "Kg");

  /** The standard unit of angles, radians. */
  public static final Angle Angle = new Angle(null, 1, "Radian", "rad");

  /** The standard "unitless" unit. */
  public static final Dimensionless Value = new Dimensionless(null, 1, "<?>", "<?>");

  /** The standard unit of voltage, volts. */
  public static final Voltage Voltage = new Voltage(null, 1, "Volt", "V");

  /** The standard unit of electric current, amperes. */
  public static final Current Current = new Current(null, 1, "Amp", "A");

  /** The standard unit of energy, joules. */
  public static final Energy Energy = new Energy(null, 1, "Joule", "J");

  /** The standard unit of power, watts. */
  public static final Power Power = new Power(null, 1, "Watt", "W");

  /** The standard unit of temperature, kelvin. */
  public static final Temperature Temperature =
      new Temperature(null, x -> x, x -> x, "Kelvin", "K");
}
