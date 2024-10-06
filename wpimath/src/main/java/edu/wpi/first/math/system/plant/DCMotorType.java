// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant;

import static edu.wpi.first.units.Units.Amp;
import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.RPM;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.Volt;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.units.AngularVelocityUnit;
import edu.wpi.first.units.CurrentUnit;
import edu.wpi.first.units.TorqueUnit;
import edu.wpi.first.units.VoltageUnit;
import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.Per;
import edu.wpi.first.units.measure.Torque;
import edu.wpi.first.units.measure.Voltage;

/** Holds the constants for a DC motor. */
public class DCMotorType {
  /** Voltage in volts at which the motor constants were measured. */
  public final double nominalVoltageVolts;

  /** Voltage at which the motor constants were measured. */
  public final Voltage nominalVoltage;

  /** Torque when stalled in Newton-Meters. */
  public final double stallTorqueNewtonMeters;

  /** Torque when stalled. */
  public final Torque stallTorque;

  /** Current draw in amps when stalled. */
  public final double stallCurrentAmps;

  /** Current draw when stalled. */
  public final Current stallCurrent;

  /** Current draw in amps under no load. */
  public final double freeCurrentAmps;

  /** Current draw under no load. */
  public final Current freeCurrent;

  /** Angular velocity in radians per second under no load. */
  public final double freeSpeedRadPerSec;

  /** Angular velocity under no load. */
  public final AngularVelocity freeSpeed;

  /** Motor internal resistance in ohms. */
  public final double rOhms;

  /** Motor internal resistance. */
  public final Per<VoltageUnit, CurrentUnit> R;

  /** Motor velocity constant in radians per second per volt. */
  public final double KvRadPerSecPerVolt;

  /** Motor velocity constant. */
  public final Per<AngularVelocityUnit, VoltageUnit> Kv;

  /** Motor torque constant in Newton-Meters per amp. */
  public final double KtNMPerAmp;

  /** Motor torque constant. */
  public final Per<TorqueUnit, CurrentUnit> Kt;

  /**
   * Constructs a DCMotorConstants object.
   *
   * @param nominalVoltage The voltage at which the motor constants were measured.
   * @param stallTorque The torque when stalled.
   * @param stallCurrent The current draw when stalled.
   * @param freeCurrent The current draw under no load.
   * @param freeSpeed The angular velocity under no load.
   * @throws IllegalArgumentException if nominalVoltageVolts &leq; 0, stallTorqueNewtonMeters &leq;
   *     0, stallCurrentAmps &leq; 0, freeCurrentAmps &leq; 0, or freeSpeedRadPerSec &leq; 0.
   */
  public DCMotorType(
      Voltage nominalVoltage,
      Torque stallTorque,
      Current stallCurrent,
      Current freeCurrent,
      AngularVelocity freeSpeed) {
    if (nominalVoltage.in(Volts) <= 0) {
      throw new IllegalArgumentException("nominalVoltage must be greater than zero.");
    }
    if (stallTorque.in(NewtonMeters) <= 0) {
      throw new IllegalArgumentException("stallTorque must be greater than zero.");
    }
    if (stallCurrent.in(Amps) <= 0) {
      throw new IllegalArgumentException("stallCurrent must be greater than zero.");
    }
    if (freeCurrent.in(Amps) <= 0) {
      throw new IllegalArgumentException("freeCurrent must be greater than zero.");
    }
    if (freeSpeed.in(RadiansPerSecond) <= 0) {
      throw new IllegalArgumentException("freeSpeed must be greater than zero.");
    }
    this.nominalVoltage = nominalVoltage;
    this.stallTorque = stallTorque;
    this.stallCurrent = stallCurrent;
    this.freeCurrent = freeCurrent;
    this.freeSpeed = freeSpeed;
    nominalVoltageVolts = nominalVoltage.in(Volts);
    stallTorqueNewtonMeters = stallTorque.in(NewtonMeters);
    stallCurrentAmps = stallCurrent.in(Amps);
    freeCurrentAmps = freeCurrent.in(Amps);
    freeSpeedRadPerSec = freeSpeed.in(RadiansPerSecond);
    R = nominalVoltage.divide(stallCurrent);
    rOhms = R.in(Volts.per(Amp));
    Kv =
        Per.ofBaseUnits(
            freeSpeedRadPerSec / (nominalVoltageVolts - rOhms * freeCurrentAmps),
            RadiansPerSecond.per(Volt));
    KvRadPerSecPerVolt = Kv.in(RadiansPerSecond.per(Volt));
    Kt = stallTorque.divide(stallCurrent);
    KtNMPerAmp = Kt.in(NewtonMeters.per(Amp));
  }

  /**
   * Constructs a DCMotorConstants object.
   *
   * @param nominalVoltageVolts The voltage at which the motor constants were measured in volts.
   * @param stallTorqueNewtonMeters The torque when stalled in Newton-Meters.
   * @param stallCurrentAmps The current draw when stalled in amps.
   * @param freeCurrentAmps The current draw under no load in amps.
   * @param freeSpeedRadPerSec The angular velocity under no load in radians per second.
   * @throws IllegalArgumentException if nominalVoltageVolts &leq; 0, stallTorqueNewtonMeters &leq;
   *     0, stallCurrentAmps &leq; 0, freeCurrentAmps &leq; 0, or freeSpeedRadPerSec &leq; 0.
   */
  public DCMotorType(
      double nominalVoltageVolts,
      double stallTorqueNewtonMeters,
      double stallCurrentAmps,
      double freeCurrentAmps,
      double freeSpeedRadPerSec) {
    this(
        Volts.of(nominalVoltageVolts),
        NewtonMeters.of(stallTorqueNewtonMeters),
        Amps.of(stallCurrentAmps),
        Amps.of(freeCurrentAmps),
        RadiansPerSecond.of(freeSpeedRadPerSec));
  }

  /** DC motor constants for a VEX CIM motor. */
  public static final DCMotorType CIM =
      new DCMotorType(
          Volts.of(12), NewtonMeters.of(2.42), Amps.of(133), Amps.of(2.7), RPM.of(5310));

  /** DC motor constants for a VEX 775Pro motor. */
  public static final DCMotorType Vex775Pro =
      new DCMotorType(
          Volts.of(12), NewtonMeters.of(0.71), Amps.of(134), Amps.of(0.7), RPM.of(18730));

  /** DC motor constants for a REV NEO brushless motor. */
  public static final DCMotorType NEO =
      new DCMotorType(Volts.of(12), NewtonMeters.of(2.6), Amps.of(105), Amps.of(1.8), RPM.of(5676));

  /** DC motor constants for a VEX MiniCIM motor. */
  public static final DCMotorType MiniCIM =
      new DCMotorType(Volts.of(12), NewtonMeters.of(1.41), Amps.of(89), Amps.of(3.0), RPM.of(5840));

  /** DC motor constants for a VEX Bag motor. */
  public static final DCMotorType Bag =
      new DCMotorType(
          Volts.of(12), NewtonMeters.of(0.43), Amps.of(53), Amps.of(1.8), RPM.of(13180));

  /** DC motor constants for a Andymark RS775-125 motor. */
  public static final DCMotorType AndymarkRs775_125 =
      new DCMotorType(
          Volts.of(12), NewtonMeters.of(0.28), Amps.of(18), Amps.of(1.6), RPM.of(5800.0));

  /** DC motor constants for a Banebots RS775 motor. */
  public static final DCMotorType BanebotsRs775 =
      new DCMotorType(
          Volts.of(12), NewtonMeters.of(0.72), Amps.of(97), Amps.of(2.7), RPM.of(13050.0));

  /** DC motor constants for a Andymark 9015 motor. */
  public static final DCMotorType Andymark9015 =
      new DCMotorType(
          Volts.of(12), NewtonMeters.of(0.36), Amps.of(71), Amps.of(3.7), RPM.of(14270.0));

  /** DC motor constants for a Banebots RS 550 motor. */
  public static final DCMotorType BanebotsRs550 =
      new DCMotorType(
          Volts.of(12), NewtonMeters.of(0.38), Amps.of(84), Amps.of(0.4), RPM.of(19000.0));

  /** DC motor constants for a REV NEO 550 brushless motor. */
  public static final DCMotorType Neo550 =
      new DCMotorType(
          Volts.of(12), NewtonMeters.of(0.97), Amps.of(100), Amps.of(1.4), RPM.of(11000.0));

  /**
   * DC motor constants for a VEX Falcon 500 brushless motor.
   *
   * <p>https://store.ctr-electronics.com/falcon-500-powered-by-talon-fx/
   */
  public static final DCMotorType Falcon500 =
      new DCMotorType(
          Volts.of(12), NewtonMeters.of(4.69), Amps.of(257), Amps.of(1.5), RPM.of(6380.0));

  /**
   * DC motor constants for a VEX Falcon 500 brushless motor with FOC (Field-Oriented Control).
   *
   * <p>https://store.ctr-electronics.com/falcon-500-powered-by-talon-fx/
   */
  public static final DCMotorType Falcon500FOC =
      new DCMotorType(
          Volts.of(12), NewtonMeters.of(5.84), Amps.of(304), Amps.of(1.5), RPM.of(6080.0));

  /**
   * DC motor constants for a Romi/TI_RSLK MAX motor.
   *
   * <p>https://www.pololu.com/product/1520/specs
   */
  public static final DCMotorType RomiBuiltIn =
      new DCMotorType(
          Volts.of(4.5), NewtonMeters.of(0.1765), Amps.of(1.25), Amps.of(0.13), RPM.of(150.0));

  /**
   * DC motor constants for a Kraken X60 brushless motor.
   *
   * <p>https://store.ctr-electronics.com/announcing-kraken-x60/
   */
  public static final DCMotorType KrakenX60 =
      new DCMotorType(Volts.of(12), NewtonMeters.of(7.09), Amps.of(366), Amps.of(2), RPM.of(6000));

  /**
   * DC motor constants for a Kraken X60 brushless motor with FOC (Field-Oriented Control) enabled.
   *
   * <p>https://store.ctr-electronics.com/announcing-kraken-x60/
   */
  public static final DCMotorType KrakenX60FOC =
      new DCMotorType(Volts.of(12), NewtonMeters.of(9.37), Amps.of(483), Amps.of(2), RPM.of(5800));

  /**
   * DC motor constants for a REV Neo Vortex brushless motor.
   *
   * <p>https://www.revrobotics.com/next-generation-spark-neo/
   */
  public static final DCMotorType NeoVortex =
      new DCMotorType(
          Volts.of(12), NewtonMeters.of(3.60), Amps.of(211), Amps.of(3.6), RPM.of(6784));
}
