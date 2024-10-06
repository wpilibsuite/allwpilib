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
import edu.wpi.first.units.measure.Torque;
import edu.wpi.first.units.measure.Voltage;

/**
 * Represents a gearbox with a non-zero number of DC motors of the same type.
 */
public record Gearbox(
    DCMotorType motorType,
    double reduction,
    int numMotors) {

  public Gearbox(
      DCMotorType motortype) {
    this(motortype, 1.0, 1);
  }

  public Gearbox(
      DCMotorType motorType,
      int numMotors) {
    this(motorType, 1.0, numMotors);
  }

  public Gearbox(
      DCMotorType motorType,
      double reduction) {
    this(motorType, reduction, 1);
  }

  /**
   * Calculates net current drawn by the motors with given speed and input
   * voltage.
   *
   * @param speed   The current angular velocity of the motor.
   * @param voltage The voltage being applied to the motor.
   * @return The estimated current.
   */
  public Current current(AngularVelocity speed, Voltage voltage) {
    return Amps.of(currentAmps(speed.in(RadiansPerSecond), voltage.in(Volts)));
  }

  /**
   * Calculates net current drawn in amps by the motors with given speed and input
   * voltage.
   *
   * @param speedRadiansPerSec The current angular velocity of the motor.
   * @param voltageInputVolts  The voltage being applied to the motor.
   * @return The estimated current in amps.
   */
  public double currentAmps(double speedRadiansPerSec, double voltageInputVolts) {
    return -numMotors * speedRadiansPerSec * reduction / motorType.kvRadPerSecPerVolt() / motorType.rOhms()
        + numMotors * voltageInputVolts / motorType.rOhms();
  }

  /**
   * Calculates net current drawn by the motors for a given torque.
   *
   * @param torque The torque produced by the motor.
   * @return The current drawn by the motor.
   */
  public Current current(Torque torque) {
    return Amps.of(currentAmps(torque.in(NewtonMeters)));
  }

  /**
   * Calculates net current in amps drawn by the motors for a given torque.
   *
   * @param torqueNm The torque produced by the motor in Newton-Meters.
   * @return The current drawn by the motor in amps.
   */
  public double currentAmps(double torqueNm) {
    return torqueNm / motorType.ktNMPerAmp() / reduction;
  }

  /**
   * Calculates the torque produced by the motors with a given current.
   *
   * @param current The net current drawn by the motors.
   * @return The torque output.
   */
  public Torque torque(Current current) {
    return NewtonMeters.of(torque(current.in(Amps)));
  }

  /**
   * Calculate the torque in Newton-Meters produced by the motors with a given
   * current.
   *
   * @param currentAmpere The net current drawn by the motors.
   * @return The torque output in Newton-Meters.
   */
  public double torque(double currentAmpere) {
    return currentAmpere * reduction * motorType.ktNMPerAmp();
  }

  /**
   * Calculate the voltage provided to the motors for a given torque and angular
   * velocity.
   *
   * @param torque The torque produced by the motor.
   * @param speed  The current angular velocity of the motor.
   * @return The voltage of the motor.
   */
  public Voltage voltage(Torque torque, AngularVelocity speed) {
    return Volts.of(voltage(torque.in(NewtonMeters), speed.in(RadiansPerSecond)));
  }

  /**
   * Calculate the voltage provided to the motors for a given torque and angular
   * velocity in volts.
   *
   * @param torqueNm           The torque produced by the motor in Newton-Meters.
   * @param speedRadiansPerSec The current angular velocity of the motor in
   *                           radians per second.
   * @return The voltage of the motor in volts.
   */
  public double voltage(double torqueNm, double speedRadiansPerSec) {
    return speedRadiansPerSec * reduction / motorType.kvRadPerSecPerVolt()
        + motorType.rOhms() * torqueNm / numMotors / reduction / motorType.ktNMPerAmp();
  }

  /**
   * Calculates the angular speed produced by the motor at a given torque and
   * input voltage.
   *
   * @param torqueNm          The torque produced by the motor.
   * @param voltageInputVolts The voltage applied to the motor.
   * @return The angular speed of the motor.
   */
  public double speed(double torqueNm, double voltageInputVolts) {
    return voltageInputVolts * motorType.kvRadPerSecPerVolt() / reduction
        - motorType.kvRadPerSecPerVolt() * motorType.rOhms() * torqueNm / numMotors / reduction / reduction
            / motorType.ktNMPerAmp();
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
