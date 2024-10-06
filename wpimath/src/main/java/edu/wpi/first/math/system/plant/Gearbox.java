// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.NewtonMeters;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.MutAngularVelocity;
import edu.wpi.first.units.measure.MutCurrent;
import edu.wpi.first.units.measure.MutTorque;
import edu.wpi.first.units.measure.MutVoltage;
import edu.wpi.first.units.measure.Torque;
import edu.wpi.first.units.measure.Voltage;

/** Represents a gearbox with a non-zero number of DC motors of the same type. */
public class Gearbox {
  /** The type of motor used in the gearbox. */
  public final DCMotorType motorType;

  /** The gearbox reduction as a ratio of output turn to input turns. */
  public final double reduction;

  /** The number of motors in the gearbox. */
  public final int numMotors;

  /** The net current of the gearbox. */
  private final MutCurrent m_current = Amps.mutable(0.0);

  /** The net torque produced by the gearbox. */
  private final MutTorque m_torque = NewtonMeters.mutable(0.0);

  /** The voltage on the gearbox. */
  private final MutVoltage m_voltage = Volts.mutable(0.0);

  /** The angular velocity of the gearbox's output. */
  private final MutAngularVelocity m_angularVelocity = RadiansPerSecond.mutable(0.0);

  /**
   * Creates a gearbox with the supplied motor type, reduction, and number of motors.
   *
   * @param motorType The type of motor used in the gearbox.
   * @param reduction The gearbox reduction as a ratio of output turns to input turns.
   * @param numMotors The number of motors used in the gearbox.
   * @throws IllegalArgumentException if reduction &leq; 0 or numMotors &leq; 0.
   */
  public Gearbox(DCMotorType motorType, double reduction, int numMotors) {
    if (reduction <= 0) {
      throw new IllegalArgumentException("reduction must be greater than zero.");
    }
    if (numMotors <= 0) {
      throw new IllegalArgumentException("numMotors must be greater than zero.");
    }
    this.motorType = motorType;
    this.reduction = reduction;
    this.numMotors = numMotors;
  }

  /**
   * Creates a gearbox with the supplied motor type. The reduction is set to 1.0 and the number of
   * motors is set to 1.
   *
   * @param motorType The type of motor used in the gearbox.
   */
  public Gearbox(DCMotorType motorType) {
    this(motorType, 1.0, 1);
  }

  /**
   * Creates a gearbox with the supplied motor type and number of motors. The reduction is set to
   * 1.0.
   *
   * @param motorType The type of motor used in the gearbox.
   * @param numMotors The number of motors used in the gearbox.
   * @throws IllegalArgumentException if numMotors &leq; 0.
   */
  public Gearbox(DCMotorType motorType, int numMotors) {
    this(motorType, 1.0, numMotors);
  }

  /**
   * Creates a gearbox with the supplied motor type and reduction. The number of motors is set to 1.
   *
   * @param motorType The type of motor used in the gearbox.
   * @param reduction The gearbox reduction as a ratio of output turns to input turns.
   */
  public Gearbox(DCMotorType motorType, double reduction) {
    this(motorType, reduction, 1);
  }

  /**
   * Calculates net current drawn by the motors with given speed and input voltage.
   *
   * @param angularVelocity The angular velocity of the motor.
   * @param voltage The voltage being applied to the motor.
   * @return The estimated current.
   */
  public Current current(AngularVelocity angularVelocity, Voltage voltage) {
    m_current.mut_replace(
        currentAmps(angularVelocity.in(RadiansPerSecond), voltage.in(Volts)), Amps);
    return m_current;
  }

  /**
   * Calculates net current drawn by the motors for a given torque.
   *
   * @param torque The torque produced by the motor.
   * @return The current drawn by the motor.
   */
  public Current current(Torque torque) {
    m_current.mut_replace(currentAmps(torque.in(NewtonMeters)), Amps);
    return m_current;
  }

  /**
   * Calculates net current drawn in amps by the motors with given speed and input voltage.
   *
   * @param speedRadiansPerSec The current angular velocity of the motor.
   * @param voltageInputVolts The voltage being applied to the motor.
   * @return The estimated current in amps.
   */
  public double currentAmps(double speedRadiansPerSec, double voltageInputVolts) {
    return -numMotors
            * speedRadiansPerSec
            * reduction
            / motorType.kvRadPerSecPerVolt
            / motorType.rOhms
        + numMotors * voltageInputVolts / motorType.rOhms;
  }

  /**
   * Calculates net current in amps drawn by the motors for a given torque.
   *
   * @param torqueNm The torque produced by the motor in Newton-Meters.
   * @return The current drawn by the motor in amps.
   */
  public double currentAmps(double torqueNm) {
    return torqueNm / motorType.ktNMPerAmp / reduction;
  }

  /**
   * Calculates the torque produced by the motors with a given current.
   *
   * @param current The net current drawn by the motors.
   * @return The torque output.
   */
  public Torque torque(Current current) {
    m_torque.mut_replace(torqueNm(current.in(Amps)), NewtonMeters);
    return m_torque;
  }

  /**
   * Calculate the torque in Newton-Meters produced by the motors with a given current.
   *
   * @param currentAmpere The net current drawn by the motors in amps.
   * @return The torque output in Newton-Meters.
   */
  public double torqueNm(double currentAmpere) {
    return currentAmpere * reduction * motorType.ktNMPerAmp;
  }

  /**
   * Calculate the voltage provided to the motors for a given torque and angular velocity.
   *
   * @param torque The torque produced by the motor.
   * @param angularVelocity The angular velocity of the motor.
   * @return The voltage of the motor.
   */
  public Voltage voltage(Torque torque, AngularVelocity angularVelocity) {
    m_voltage.mut_replace(
        voltageVolts(torque.in(NewtonMeters), angularVelocity.in(RadiansPerSecond)), Volts);
    return m_voltage;
  }

  /**
   * Calculate the voltage provided to the motors for a given torque and angular velocity in volts.
   *
   * @param torqueNm The torque produced by the motor in Newton-Meters.
   * @param angularVelocityRadPerSec The angular velocity of the motor in radians per second.
   * @return The voltage of the motor in volts.
   */
  public double voltageVolts(double torqueNm, double angularVelocityRadPerSec) {
    return angularVelocityRadPerSec * reduction / motorType.kvRadPerSecPerVolt
        + motorType.rOhms * torqueNm / numMotors / reduction / motorType.ktNMPerAmp;
  }

  /**
   * Calculates the angular speed produced by the motor at a given torque and input voltage.
   *
   * @param torque The torque produced by the motor.
   * @param voltage The voltage applied to the motor.
   * @return The angular speed of the motor.
   */
  public AngularVelocity angularVelocity(Torque torque, Voltage voltage) {
    m_angularVelocity.mut_replace(
        angularVelocityRadPerSec(torque.in(NewtonMeters), voltage.in(Volts)), RadiansPerSecond);
    return m_angularVelocity;
  }

  /**
   * Calculates the angular speed produced by the motor at a given torque and input voltage in
   * radians per second.
   *
   * @param torqueNm The torque produced by the motor in Newton-Meters.
   * @param voltageInputVolts The voltage applied to the motor in volts.
   * @return The angular speed of the motor.
   */
  public double angularVelocityRadPerSec(double torqueNm, double voltageInputVolts) {
    return voltageInputVolts * motorType.kvRadPerSecPerVolt / reduction
        - motorType.kvRadPerSecPerVolt
            * motorType.rOhms
            * torqueNm
            / numMotors
            / reduction
            / reduction
            / motorType.ktNMPerAmp;
  }

  /**
   * Returns a copy of this motor with the given gearbox reduction applied.
   *
   * @param reduction The gearbox reduction.
   * @return A motor with the gearbox reduction applied.
   */
  public Gearbox withReduction(double reduction) {
    return new Gearbox(motorType, reduction, numMotors);
  }
}
