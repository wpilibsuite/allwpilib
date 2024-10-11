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
  /** The wheel's gearbox. */
  public final Gearbox gearbox;

  /** The radius of the wheel in meters. */
  public final double radiusMeters;

  /** The radius of the wheel. */
  public final Distance radius;

  /** The net current of the wheel. */
  private final MutCurrent m_current = Amps.mutable(0.0);

  /** The net torque produced by the wheel. */
  private final MutForce m_force = Newtons.mutable(0.0);

  /** The net force produced by the wheel */

  /** The voltage on the wheel. */
  private final MutVoltage m_voltage = Volts.mutable(0.0);

  /** The linear velocity of the wheel. */
  private final MutLinearVelocity m_velocity = MetersPerSecond.mutable(0.0);

  /**
   * Creates a wheel with the supplied gearbox and radius in meters.
   *
   * @param gearbox The wheel's gearbox.
   * @param radiusMeters The wheel's radius in meters.
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
   * Creates a wheel with the supplied gearbox and radius in meters.
   *
   * @param gearbox The wheel's gearbox.
   * @param radius The wheel's radius.
   * @throws IllegalArgumentException if radius &leq; 0.
   */
  public Wheel(Gearbox gearbox, Distance radius) {
    this(gearbox, radius.in(Meters));
  }

  /**
   * Calculates net current drawn by the motors with given speed and input voltage.
   *
   * @param velocity The linear velocity of the motor.
   * @param voltage The voltage being applied to the motor.
   * @return The estimated current.
   */
  public Current current(LinearVelocity velocity, Voltage voltage) {
    m_current.mut_replace(currentAmps(velocity.in(MetersPerSecond), voltage.in(Volts)), Amps);
    return m_current;
  }

  /**
   * Calculates net current drawn by the motors for a given force.
   *
   * @param force The force produced by the motor.
   * @return The net current drawn by the motors.
   */
  public Current current(Force force) {
    m_current.mut_replace(currentAmps(force.in(Newtons)), Amps);
    return m_current;
  }

  /**
   * Calculates net current drawn in amps by the motors with given speed and input voltage.
   *
   * @param velocityMetersPerSecond The linear velocity of the motor in meters per second.
   * @param voltageInputVolts The voltage being applied to the motor in volts.
   * @return The estimated current in amps.
   */
  public double currentAmps(double velocityMetersPerSecond, double voltageInputVolts) {
    return gearbox.currentAmps(velocityMetersPerSecond / radiusMeters, voltageInputVolts);
  }

  /**
   * Calculates net current in amps drawn by the motors for a given force.
   *
   * @param forceNewtons The force produced by the motor in Newtons.
   * @return The current drawn by the motor in amps.
   */
  public double currentAmps(double forceNewtons) {
    return gearbox.currentAmps(forceNewtons * radiusMeters);
  }

  /**
   * Calculates the torque produced by the motors with a given current.
   *
   * @param current The net current drawn by the motors.
   * @return The torque output.
   */
  public Force force(Current current) {
    m_force.mut_replace(forceNewtons(current.in(Amps)), Newtons);
    return m_force;
  }

  /**
   * Calculate the force in Newtons produced by the motors with a given current.
   *
   * @param currentAmpere The net current drawn by the motors in amps.
   * @return The force output in Newtons.
   */
  public double forceNewtons(double currentAmpere) {
    return gearbox.torqueNewtonMeters(currentAmpere) / radiusMeters;
  }

  /**
   * Calculate the voltage provided to the wheel for a given force and linear velocity.
   *
   * @param force The force produced by the wheel.
   * @param velocity The linear velocity of the motor.
   * @return The voltage of the motor.
   */
  public Voltage voltage(Force force, LinearVelocity velocity) {
    m_voltage.mut_replace(voltageVolts(force.in(Newtons), velocity.in(MetersPerSecond)), Volts);
    return m_voltage;
  }

  /**
   * Calculate the voltage provided to the motors for a given force in Newtons and linear velocity
   * in meters per second.
   *
   * @param forceNewtons The force produced by the motor in Newtons.
   * @param velocityMetersPerSecond The linear velocity of the motor in meters per second.
   * @return The voltage of the motor in volts.
   */
  public double voltageVolts(double forceNewtons, double velocityMetersPerSecond) {
    return gearbox.voltage(forceNewtons * radiusMeters, velocityMetersPerSecond / radiusMeters);
  }

  /**
   * Calculates the linear speed produced by the motor at a given force and input voltage.
   *
   * @param force The force produced by the motor.
   * @param voltage The voltage applied to the motor.
   * @return The linear speed of the motor.
   */
  public LinearVelocity velocity(Force force, Voltage voltage) {
    m_velocity.mut_replace(
        velocityMetersPerSecond(force.in(Newtons), voltage.in(Volts)), MetersPerSecond);
    return m_velocity;
  }

  /**
   * Calculates the linear speed produced by the motor at a given force in Newtons and input voltage
   * in volts.
   *
   * @param forceNewtons The force produced by the motor in Newtons.
   * @param voltageInputVolts The voltage applied to the motor in volts.
   * @return The angular speed of the motor.
   */
  public double velocityMetersPerSecond(double forceNewtons, double voltageInputVolts) {
    return gearbox.angularVelocityRadPerSec(forceNewtons * radiusMeters, voltageInputVolts);
  }
}
