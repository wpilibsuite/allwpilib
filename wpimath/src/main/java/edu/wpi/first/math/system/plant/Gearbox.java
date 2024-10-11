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
  public final DCMotor motorType;

  /** The gearbox reduction as a ratio of output turns to input turns. */
  public final double reduction;

  /** The number of motors in the gearbox. */
  public final int numMotors;

  /** The net current of the gearbox. */
  private final MutCurrent m_current = Amps.mutable(0.0);

  /** The net torque of the gearbox. */
  private final MutTorque m_torque = NewtonMeters.mutable(0.0);

  /** The voltage of the gearbox. */
  private final MutVoltage m_voltage = Volts.mutable(0.0);

  /** The angular velocity of the gearbox. */
  private final MutAngularVelocity m_angularVelocity = RadiansPerSecond.mutable(0.0);

  /**
   * Creates a gearbox.
   *
   * @param numMotors The number of motors used in the gearbox.
   * @param motorType The type of motor used in the gearbox.
   * @param reduction The gearbox reduction as a ratio of output turns to input turns.
   * @throws IllegalArgumentException if reduction &leq; 0 or numMotors &leq; 0.
   */
  public Gearbox(int numMotors, DCMotor motorType, double reduction) {
    if (numMotors <= 0) {
      throw new IllegalArgumentException("numMotors must be greater than zero.");
    }
    if (reduction <= 0) {
      throw new IllegalArgumentException("reduction must be greater than zero.");
    }
    this.motorType = motorType;
    this.reduction = reduction;
    this.numMotors = numMotors;
  }

  /**
   * Creates a gearbox.
   *
   * <p>The reduction is set to 1.0 and the number of motors is set to 1.
   *
   * @param motorType The type of motor used in the gearbox.
   */
  public Gearbox(DCMotor motorType) {
    this(1, motorType, 1.0);
  }

  /**
   * Creates a gearbox.
   *
   * <p>The reduction is set to 1.0.
   *
   * @param numMotors The number of motors used in the gearbox.
   * @param motorType The type of motor used in the gearbox.
   * @throws IllegalArgumentException if numMotors &leq; 0.
   */
  public Gearbox(int numMotors, DCMotor motorType) {
    this(numMotors, motorType, 1.0);
  }

  /**
   * Creates a gearbox.
   *
   * <p>The number of motors is set to 1.
   *
   * @param motorType The type of motor used in the gearbox.
   * @param reduction The gearbox reduction as a ratio of output turns to input turns.
   */
  public Gearbox(DCMotor motorType, double reduction) {
    this(1, motorType, reduction);
  }

  /**
   * Calculates the estimated net current drawn by the motors.
   *
   * @param angularVelocityRadPerSec The angular velocity of the output of the gearbox in radians
   *     per second.
   * @param volts The voltage applied to the motors in volts.
   * @return The estimated net current drawn by the motors in amps.
   */
  public double currentAmps(double angularVelocityRadPerSec, double volts) {
    return -numMotors
            * angularVelocityRadPerSec
            * reduction
            / motorType.KvRadPerSecPerVolt
            / motorType.rOhms
        + numMotors * volts / motorType.rOhms;
  }

  /**
   * Calculates the net current drawn by the motors.
   *
   * @param torqueNewtonMeters The torque produced by the gearbox in newton-meters.
   * @return The estimated net current drawn by the motors in amps.
   */
  public double currentAmps(double torqueNewtonMeters) {
    return torqueNewtonMeters / motorType.KtNMPerAmp / reduction;
  }

  /**
   * Calculates the estimated net current drawn by the motors.
   *
   * @param angularVelocity The angular velocity of the output of the gearbox.
   * @param voltage The voltage applied to the motors.
   * @return The estimated net current drawn by the motors.
   */
  public Current current(AngularVelocity angularVelocity, Voltage voltage) {
    m_current.mut_replace(
        currentAmps(angularVelocity.in(RadiansPerSecond), voltage.in(Volts)), Amps);
    return m_current;
  }

  /**
   * Calculates the estimated net current drawn by the motors.
   *
   * @param torque The torque produced by the gearbox.
   * @return The estimated net current drawn by the motors.
   */
  public Current current(Torque torque) {
    m_current.mut_replace(currentAmps(torque.in(NewtonMeters)), Amps);
    return m_current;
  }

  /**
   * Calculates the estimated net torque produced by the gearbox.
   *
   * @param currentAmpere The net current drawn by the motors in amps.
   * @return The estimated net torque produced by the gearbox in newton-meters.
   */
  public double torqueNewtonMeters(double currentAmpere) {
    return currentAmpere * reduction * motorType.KtNMPerAmp;
  }

  /**
   * Calculates the estimated net torque produced by the gearbox.
   *
   * @param current The net current drawn by the motors.
   * @return The estimated net torque produced by the gearbox.
   */
  public Torque torque(Current current) {
    m_torque.mut_replace(torqueNewtonMeters(current.in(Amps)), NewtonMeters);
    return m_torque;
  }

  /**
   * Calculate the estimated voltage on the motors.
   *
   * @param torqueNm The torque produced by the gearbox in newton-meters.
   * @param angularVelocityRadPerSec The angular velocity of the gearbox in radians per second.
   * @return The voltage of the motor in volts.
   */
  public double voltage(double torqueNm, double angularVelocityRadPerSec) {
    return angularVelocityRadPerSec * reduction / motorType.KvRadPerSecPerVolt
        + motorType.rOhms * torqueNm / numMotors / reduction / motorType.KtNMPerAmp;
  }

  /**
   * Calculate the estimated voltage on the motors.
   *
   * @param torque The torque produced by the gearbox.
   * @param angularVelocity The angular velocity of the gearbox.
   * @return The voltage of the motors.
   */
  public Voltage voltage(Torque torque, AngularVelocity angularVelocity) {
    m_voltage.mut_replace(
        voltage(torque.in(NewtonMeters), angularVelocity.in(RadiansPerSecond)), Volts);
    return m_voltage;
  }

  /**
   * Calculates the estimated angular speed of the output of the gearbox.
   *
   * @param torqueNewtonMeters The torque produced by the gearbox in newton-meters.
   * @param voltage The voltage of the motors in volts.
   * @return The angular speed of the output of the gearbox in radians per second.
   */
  public double angularVelocityRadPerSec(double torqueNewtonMeters, double voltage) {
    return voltage * motorType.KvRadPerSecPerVolt / reduction
        - motorType.KvRadPerSecPerVolt
            * motorType.rOhms
            * torqueNewtonMeters
            / numMotors
            / reduction
            / reduction
            / motorType.KtNMPerAmp;
  }

  /**
   * Calculates the estimated angular speed of the output of the gearbox.
   *
   * @param torque The torque produced by the gearbox.
   * @param voltage The voltage of the motors.
   * @return The angular speed of the output of the gearbox.
   */
  public AngularVelocity angularVelocity(Torque torque, Voltage voltage) {
    m_angularVelocity.mut_replace(
        angularVelocityRadPerSec(torque.in(NewtonMeters), voltage.in(Volts)), RadiansPerSecond);
    return m_angularVelocity;
  }

  /**
   * Returns a copy of this motor with the given gearbox reduction applied.
   *
   * @param reduction The gearbox reduction.
   * @return A motor with the gearbox reduction applied.
   */
  public Gearbox withReduction(double reduction) {
    return new Gearbox(numMotors, motorType, reduction);
  }
}
