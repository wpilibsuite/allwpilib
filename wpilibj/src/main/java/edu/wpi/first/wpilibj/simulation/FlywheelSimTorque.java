// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;

/** Represents a simulated flywheel mechanism. */
public class FlywheelSimTorque extends FlywheelSim {

  /**
   * Creates a simulated flywheel mechanism.
   *
   * @param plant The linear system that represents the flywheel. Use either {@link
   *     LinearSystemId#createFlywheelSystem(DCMotor, double, double)} if using physical constants
   *     or {@link LinearSystemId#identifyVelocitySystem(double, double)} if using system
   *     characterization.
   * @param gearbox The type of and number of motors in the flywheel gearbox.
   * @param measurementStdDevs The standard deviations of the measurements. Can be omitted if no
   *     noise is desired. If present must have 1 element for velocity.
   */
  public FlywheelSimTorque(
      LinearSystem<N1, N1, N1> plant, DCMotor gearbox, double... measurementStdDevs) {
    super(plant, gearbox, measurementStdDevs);
  }

  /**
   * Sets the input torque for the flywheel.
   *
   * @param volts The input torque.
   */
  public void setInputTorque(double torqueNM) {
    setInput(torqueNM);
  }

  @Override
  public double getVoltage() {
    // TODO Auto-generated method stub
    throw new UnsupportedOperationException("Unimplemented method 'getVoltage'");
  }

  @Override
  public double getTorqueNewtonMeters() {
    // TODO Auto-generated method stub
    throw new UnsupportedOperationException("Unimplemented method 'getTorqueNewtonMeters'");
  }
}
