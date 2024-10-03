// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;

/** Represents a simulated flywheel mechanism controlled by torque input. */
public class FlywheelSimTorque extends FlywheelSim {

  /**
   * Creates a simulated flywheel mechanism controlled by torque input.
   *
   * @param plant The linear system that represents the flywheel controlled by torque input. Use either {@link
   *     LinearSystemId#createFlywheelTorqueSystem(DCMotor, double, double)} if using physical constants
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

  /**
   * Gets the voltage of the flywheel.
   *
   * @return The flywheel's voltage.
   */  
  @Override
  public double getVoltage() {
    // I = V / R - omega / (Kv * R)
    // torque = J * omega
    // V = R * torque / J + omega / Kv
    // Reductions are output over input, so a reduction of 2:1 means the motor is
    // spinning
    // 2x faster than the flywheel
    return m_gearbox.getVoltage(
      m_u.get(0, 0), 
      m_x.get(0, 0) * m_gearing) * Math.signum(m_u.get(0, 0));

  }

  /**
   * Gets the torque on the flywheel.
   *
   * @return The flywheel's torque in Newton-Meters.
   */
  @Override
  public double getTorqueNewtonMeters() {
        // I = V / R - omega / (Kv * R)
    // Reductions are output over input, so a reduction of 2:1 means the motor is
    // spinning
    // 2x faster than the flywheel 
    return getInput(0);

  }
}
