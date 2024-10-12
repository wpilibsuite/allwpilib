// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.Newtons;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.Force;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.MutCurrent;
import edu.wpi.first.units.measure.MutForce;
import edu.wpi.first.units.measure.MutLinearVelocity;
import edu.wpi.first.units.measure.MutVoltage;
import edu.wpi.first.units.measure.Voltage;

/** Represents a wheel. */
public class Wheel {
  /** The gearbox of the wheel. */
  public final Gearbox gearbox;

  /** The radius of the wheel in meters. */
  public final double radiusMeters;

  /** The radius of the wheel. */
  public final Distance radius;

  /** The net current of the wheel. */
  private final MutCurrent m_current = Amps.mutable(0.0);

  /** The net force produced by the wheel. */
  private final MutForce m_force = Newtons.mutable(0.0);

  /** The voltage on the wheel. */
  private final MutVoltage m_voltage = Volts.mutable(0.0);

  /** The linear velocity of the wheel. */
  private final MutLinearVelocity m_velocity = MetersPerSecond.mutable(0.0);

  /**
   * Creates a wheel.
   *
   * @param gearbox The gearbox of the wheel
   * @param radiusMeters The radius of the wheel in meters.
   * @throws IllegalArgumentException if radiusMeters &leq; 0.
   */
  public Wheel(Gearbox gearbox, double radiusMeters) {
    if (radiusMeters <= 0) {
      throw new IllegalArgumentException("radiusMeters must be greater than zero.");
    }
    this.gearbox = gearbox;
    this.radiusMeters = radiusMeters;
    radius = Meters.of(radiusMeters);
  }

  /**
   * Creates a wheel.
   *
   * @param gearbox The gearbox of the wheel
   * @param radius The radius of the wheel.
   * @throws IllegalArgumentException if radius &leq; 0.
   */
  public Wheel(Gearbox gearbox, Distance radius) {
    this(gearbox, radius.in(Meters));
  }

  /**
   * Calculates the estimated net current drawn by the motors.
   *
   * @param velocityMetersPerSecond The linear velocity of the wheel in meters per second.
   * @param voltageInputVolts The voltage applied to the motors in volts.
   * @return The estimated net current drawn by the motors in amps.
   */
  public double currentAmps(double velocityMetersPerSecond, double voltageInputVolts) {
    return gearbox.currentAmps(velocityMetersPerSecond / radiusMeters, voltageInputVolts);
  }

  /**
   * Calculates the estimated net current drawn by the motors.
   *
   * @param forceNewtons The force produced by the wheel in newtons.
   * @return The estimated net current drawn by the motors in amps.
   */
  public double currentAmps(double forceNewtons) {
    return gearbox.currentAmps(forceNewtons * radiusMeters);
  }

  /**
   * Calculates the estimated net current drawn by the motors.
   *
   * @param velocity The linear velocity of the motor.
   * @param voltage The voltage applied to the motors.
   * @return The estimated net current drawn by the motors.
   */
  public Current current(LinearVelocity velocity, Voltage voltage) {
    m_current.mut_replace(currentAmps(velocity.in(MetersPerSecond), voltage.in(Volts)), Amps);
    return m_current;
  }

  /**
   * Calculates the estimated net current drawn by the motors.
   *
   * @param force The force produced by the wheel.
   * @return The estimated net current drawn by the motors.
   */
  public Current current(Force force) {
    m_current.mut_replace(currentAmps(force.in(Newtons)), Amps);
    return m_current;
  }

  /**
   * Calculates the force produced by the wheel.
   *
   * @param currentAmpere The net current drawn by the motors in amps.
   * @return The force produced by the wheel in newtons.
   */
  public double forceNewtons(double currentAmpere) {
    return gearbox.torqueNewtonMeters(currentAmpere) / radiusMeters;
  }

  /**
   * Calculates the force produced by the wheel.
   *
   * @param current The net current drawn by the motors.
   * @return The force produced by the wheel.
   */
  public Force force(Current current) {
    m_force.mut_replace(forceNewtons(current.in(Amps)), Newtons);
    return m_force;
  }

  /**
   * Calculates the estimated voltage on the motors.
   *
   * @param forceNewtons The force produced by the wheel in newtons.
   * @param velocityMetersPerSecond The linear velocity of the wheel in meters per second.
   * @return The voltage of the motors in volts.
   */
  public double voltage(double forceNewtons, double velocityMetersPerSecond) {
    return gearbox.voltage(forceNewtons * radiusMeters, velocityMetersPerSecond / radiusMeters);
  }

  /**
   * Calculates the estimated voltage on the motors.
   *
   * @param force The force produced by the wheel.
   * @param velocity The linear velocity of the wheel.
   * @return The voltage of the motors.
   */
  public Voltage voltage(Force force, LinearVelocity velocity) {
    m_voltage.mut_replace(voltage(force.in(Newtons), velocity.in(MetersPerSecond)), Volts);
    return m_voltage;
  }

  /**
   * Calculates the estimated linear speed of the wheel.
   *
   * @param forceNewtons The force produced by the wheel in newtons.
   * @param voltage The voltage of the motors in volts.
   * @return The linear speed of the motor in meters per second.
   */
  public double velocityMetersPerSecond(double forceNewtons, double voltage) {
    return gearbox.angularVelocityRadPerSec(forceNewtons * radiusMeters, voltage);
  }

  /**
   * Calculates the estimated linear speed of the wheel.
   *
   * @param force The force produced by the wheel.
   * @param voltage The voltage of the motors.
   * @return The linear speed of the motor.
   */
  public LinearVelocity velocity(Force force, Voltage voltage) {
    m_velocity.mut_replace(
        velocityMetersPerSecond(force.in(Newtons), voltage.in(Volts)), MetersPerSecond);
    return m_velocity;
  }
}
