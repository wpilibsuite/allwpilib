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

/** Models the behavior of a gearbox. */
public class Gearbox {
  /** The DCMotor for this gearbox. */
  public final DCMotor dcMotor;
  /** The number of motors for this gearbox. */
  public final int numMotors;
  /** The reduction of this gear box as a ratio of output rotations to input rotations. */
  private double gearboxReduction;

  /** The returned measure of angular velocity. Exists to minimize calls to GC. */
  private final MutAngularVelocity angularVelocity = RadiansPerSecond.mutable(0.0);

  /** The returned measure of input voltage. Exists to minimize calls to GC. */
  private final MutVoltage voltageInput = Volts.mutable(0.0);

  /** The returned measure of torque. Exists to minimize calls to GC. */
  private final MutTorque torque = NewtonMeters.mutable(0.0);

  /** The returned measure of current. Exists to minimize calls to GC. */
  private final MutCurrent current = Amps.mutable(0.0);

  /**
   * Constructs a Gearbox.
   *
   * @param dcMotor The DC motor used in this gearbox.
   */
  public Gearbox(DCMotor dcMotor) {
    this(dcMotor, 1, 1.0);
  }

  /**
   * Constructs a Gearbox.
   *
   * @param dcMotor The DC motor used in this gearbox.
   * @param numMotors Number of identical motors in the gearbox.
   */
  public Gearbox(DCMotor dcMotor, int numMotors) {
    this(dcMotor, numMotors, 1.0);
  }

  /**
   * Constructs a Gearbox.
   *
   * @param dcMotor The DC motor used in this gearbox.
   * @param gearboxReduction The gearbox reduction.
   */
  public Gearbox(DCMotor dcMotor, double gearboxReduction) {
    this(dcMotor, 1, gearboxReduction);
  }

  /**
   * Constructs a Gearbox.
   *
   * @param dcMotor The DC motor used in this gearbox.
   * @param numMotors Number of identical motors in the gearbox.
   * @param gearboxReduction The gearbox reduction.
   * @throws IllegalArgumentException if numMotors &lt; 1, gearboxReduction &le; 0.
   */
  public Gearbox(DCMotor dcMotor, int numMotors, double gearboxReduction) {
    if (numMotors < 1) {
      throw new IllegalArgumentException("numMotors must be greater than or equal to 1.");
    }
    if (gearboxReduction <= 0) {
      throw new IllegalArgumentException("gearboxReduction must be greater than zero.");
    }
    this.dcMotor = dcMotor;
    this.numMotors = numMotors;
    this.gearboxReduction = gearboxReduction;
  }

  /**
   * Returns the gearbox reduction as a ratio of output rotations to input rotations.
   *
   * @return The gearbox reduction.
   */
  public double getGearboxReduction() {
    return gearboxReduction;
  }

  /**
   * Returns this gearbox with the given gearbox reduction applied.
   *
   * @param gearboxReduction The gearbox reduction.
   * @return A motor with the gearbox reduction applied.
   */
  public Gearbox withReduction(double gearboxReduction) {
    this.gearboxReduction = gearboxReduction;
    return this;
  }

  /**
   * Calculate the input voltage in Volts of each motor for a given torque and angular velocity.
   *
   * @param torqueNewtonMeters The torque produced by the gearbox.
   * @param angularVelocityRadiansPerSec The current angular velocity of the gearbox's output.
   * @return The voltage of each motor.
   */
  public double getVoltageInputVolts(
      double torqueNewtonMeters, double angularVelocityRadiansPerSec) {
    return dcMotor.getAngularVelocityRadiansPerSecond(
        torqueNewtonMeters / gearboxReduction / numMotors,
        angularVelocityRadiansPerSec * gearboxReduction);
  }

  /**
   * Calculate the input voltage of each motor for a given torque and angular velocity.
   *
   * @param torque The torque produced by the gearbox.
   * @param angularVelocity The current angular velocity of the gearbox's output.
   * @return The voltage of each motor.
   */
  public Voltage getVoltageInput(Torque torque, AngularVelocity angularVelocity) {
    return voltageInput.mut_setMagnitude(
        getVoltageInputVolts(torque.baseUnitMagnitude(), angularVelocity.baseUnitMagnitude()));
  }

  /**
   * Calculates the angular velocity in Radians per Second produced by the gearbox at a given torque
   * and input voltage.
   *
   * @param torqueNewtonMeters The torque produced by the gearbox.
   * @param voltageInputVolts The voltage applied to each motor.
   * @return The angular velocity of the gearbox's output.
   */
  public double getAngularVelocityRadiansPerSecond(
      double torqueNewtonMeters, double voltageInputVolts) {
    return dcMotor.getAngularVelocityRadiansPerSecond(
            torqueNewtonMeters / gearboxReduction / numMotors, voltageInputVolts)
        / gearboxReduction;
  }

  /**
   * Calculates the angular velocity produced by the gearbox at a given torque and input voltage.
   *
   * @param torque The torque produced by the gearbox.
   * @param voltageInput The voltage applied to each motor.
   * @return The angular velocity of the gearbox's output.
   */
  public AngularVelocity getAngularVelocity(Torque torque, Voltage voltageInput) {
    return angularVelocity.mut_setMagnitude(
        getAngularVelocityRadiansPerSecond(
            torque.baseUnitMagnitude(), voltageInput.baseUnitMagnitude()));
  }

  /**
   * Calculate torque in Newton-Meters produced by the gearbox at a given angular velocity and input
   * voltage.
   *
   * @param angularVelocityRadiansPerSec The current angular velocity of the gearbox's output.
   * @param voltageInputVolts The voltage applied to each motor.
   * @return The torque output of the gearbox.
   */
  public double getTorqueNewtonMeters(
      double angularVelocityRadiansPerSec, double voltageInputVolts) {
    return numMotors
        * gearboxReduction
        * dcMotor.getTorqueNewtonMeters(
            angularVelocityRadiansPerSec * gearboxReduction, voltageInputVolts);
  }

  /**
   * Calculate torque in produced by the gearbox at a given angular velocity and input voltage.
   *
   * @param angularVelocity The current angular velocity of the gearbox's output.
   * @param voltageInput The voltage applied to each motor.
   * @return The torque output of the gearbox.
   */
  public Torque getTorque(AngularVelocity angularVelocity, Voltage voltageInput) {
    return torque.mut_setMagnitude(
        getTorqueNewtonMeters(
            angularVelocity.baseUnitMagnitude(), voltageInput.baseUnitMagnitude()));
  }

  /**
   * Calculate net current drawn in Amps by all of the motors for a given torque.
   *
   * @param torqueNewtonMeters The torque produced by the gearbox.
   * @return The net current drawn by the all of the motors.
   */
  public double getNetCurrentAmps(double torqueNewtonMeters) {
    return numMotors * dcMotor.getCurrentAmps(torqueNewtonMeters / numMotors / gearboxReduction);
  }

  /**
   * Calculate net current drawn by all of the motors for a given torque.
   *
   * @param torque The torque produced by the gearbox.
   * @return The net current drawn by the all of the motors.
   */
  public Current getNetCurrent(Torque torque) {
    return current.mut_setBaseUnitMagnitude(getNetCurrentAmps(torque.baseUnitMagnitude()));
  }
}
