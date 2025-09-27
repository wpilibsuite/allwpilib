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
  public static final DistanceUnit DistanceUnit = new DistanceUnit(null, 1, "Meter", "m");

  /** The standard unit of time, seconds. */
  public static final TimeUnit TimeUnit = new TimeUnit(null, 1, "Second", "s");

  /** The standard unit of mass, kilograms. */
  public static final MassUnit MassUnit = new MassUnit(null, 1, "Kilogram", "Kg");

  /** The standard unit of angles, radians. */
  public static final AngleUnit AngleUnit = new AngleUnit(null, 1, "Radian", "rad");

  /** The standard "unitless" unit. */
  public static final DimensionlessUnit Value = new DimensionlessUnit(null, 1, "<?>", "<?>");

  /** The standard unit of voltage, volts. */
  public static final VoltageUnit VoltageUnit = new VoltageUnit(null, 1, "Volt", "V");

  /** The standard unit of electric current, amperes. */
  public static final CurrentUnit CurrentUnit = new CurrentUnit(null, 1, "Amp", "A");

  /** The standard unit of energy, joules. */
  public static final EnergyUnit EnergyUnit = new EnergyUnit(null, 1, "Joule", "J");

  /** The standard unit of temperature, kelvin. */
  public static final TemperatureUnit TemperatureUnit =
      new TemperatureUnit(null, x -> x, x -> x, "Kelvin", "K");
}
