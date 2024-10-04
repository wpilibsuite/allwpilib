// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated DC motor mechanism. */
public class DCMotorVoltageSim extends DCMotorSim {

  /**
   * Creates a simulated DC motor mechanism.
   *
   * @param plant              The linear system representing the DC motor. This
   *                           system can be created with
   *                           {@link edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(DCMotor, double,
   *                           double)} or {@link
   *                           edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(double, double)}.
   *                           If
   *                           {@link edu.wpi.first.math.system.plant.LinearSystemId#createDCMotorSystem(double, double)}
   *                           is used, the distance unit must be radians.
   * @param gearbox            The type of and number of motors in the DC motor
   *                           gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be
   *                           omitted if no
   *                           noise is desired. If present must have 2 elements.
   *                           The first element is for position. The
   *                           second element is for velocity.
   */
  public DCMotorVoltageSim(LinearSystem<N2, N1, N2> plant, DCMotor gearbox, double... measurementStdDevs) {
    super(plant, gearbox, measurementStdDevs);
  }

  /**
   * Sets the input voltage for the DC motor.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
  }

  /**
   * Returns the DC motor's torque in Newton-Meters.
   *
   * @return The DC motor's torque in Newton-Meters.
   */
  @Override
  public double getTorqueNewtonMeters() {
    return getAngularAccelerationRadPerSecSq() * m_jKgMetersSquared;

  }

  /**
   * Returns the voltage of the DC motor.
   *
   * @return The DC motor's voltage.
   */
  @Override
  public double getVoltage() {
    return getInput(0);
  }
}