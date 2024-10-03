// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.RobotController;

/** Represents a simulated flywheel mechanism controlled by voltage input. */
public class FlywheelSimVoltage extends FlywheelSim {

  /**
   * Creates a simulated flywheel mechanism controlled by voltage input.
   *
   * @param plant              The linear system that represents the flywheel controlled by voltage input. Use
   *                           either {@link
   *                           LinearSystemId#createFlywheelVoltageSystem(DCMotor, double, double)}
   *                           if using physical constants
   *                           or
   *                           {@link LinearSystemId#identifyVelocitySystem(double, double)}
   *                           if using system
   *                           characterization.
   * @param gearbox            The type of and number of motors in the flywheel
   *                           gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be
   *                           omitted if no
   *                           noise is desired. If present must have 1 element
   *                           for velocity.
   */
  public FlywheelSimVoltage(
      LinearSystem<N1, N1, N1> plant, DCMotor gearbox, double... measurementStdDevs) {
    super(plant, gearbox, measurementStdDevs);

  }

  /**
   * Sets the input voltage for the flywheel.
   *
   * @param volts The input voltage.
   */
  public void setInputVoltage(double volts) {
    setInput(volts);
    clampInput(RobotController.getBatteryVoltage());
  }

  /**
   * Gets the voltage of the flywheel.
   *
   * @return The flywheel's voltage.
   */
  @Override
  public double getVoltage() {
    return getInput(0);
  }

  /**
   * Gets the torque on the flywheel.
   *
   * @return The flywheel's torque in Newton-Meters.
   */
  @Override
  public double getTorqueNewtonMeters() {
    return getAngularAccelerationRadPerSecSq() * m_jKgMetersSquared;
  }
}
